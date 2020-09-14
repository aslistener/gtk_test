#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <poll.h>
#include <stdint.h>
#include <math.h>

#include <errno.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

#define MAX_TOUCHES 10

static void usage(void)
{
    printf("Usage: %s [--with-ownership]\n", program_invocation_short_name);
    printf("	Grey window: normal touch surface, with or without ownership\n");
    printf("	Upper black bar left: grabs the touchpoint, no ownership, accepts\n");
    printf("	Upper White bar left: grabs the touchpoint, no ownership, rejects\n");
    printf("	Lower black bar left: grabs the touchpoint, with ownership, accepts\n");
    printf("	Lower White bar left: grabs the touchpoint, with ownership, rejects\n");
}

enum TouchState {
    TSTATE_END = 0,
    TSTATE_BEGIN,
    TSTATE_UPDATE,
};

struct touchpoint {
    enum TouchState state;
    uint32_t touchid; /* 0 == unused */
    double x, y;   /* last recorded x/y position */
    Bool owned;
};

struct grabpoint {
    enum TouchState state;
    uint32_t touchid;
    double startx, starty; /* x/y of TouchBegin */
    double x, y;           /* last recorded x/y position */
    Window grab_window;
    int nevents;
};

struct multitouch {
    Display *dpy;
    int screen_no;
    Screen* screen;
    Window root;
    Window win;
    Window blackbar;
    Window whitebar;
    Window blackbar_os; /* with ownership */
    Window whitebar_os; /* with ownership */
    GC gc;
    Visual *visual;
    int xi_opcode;

    Pixmap pixmap;

    int width;
    int height;

    cairo_t *cr;
    cairo_t *cr_win;
    cairo_t *cr_grabs;
    cairo_surface_t *surface;
    cairo_surface_t *surface_win;
    cairo_surface_t *surface_grabs;

    struct touchpoint touches[MAX_TOUCHES];
    int ntouches;

    struct grabpoint grabs[MAX_TOUCHES];
    size_t ngrabs;

    Bool needs_ownership;
};

static void expose(struct multitouch *mt, int x1, int y1, int x2, int y2);

static int error(const char *fmt, ...)
{
    va_list args;
    fprintf(stderr, "E: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    return EXIT_FAILURE;
}

static void msg(const char *fmt, ...)
{
    va_list args;
    printf("M: ");

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

static int running = 1;
static void sighandler(int signal)
{
    running = 0;
    error("signal received, shutting down\n");
}

static void init_windows(struct multitouch *mt, Bool ownership)
{
    Window win, subwin;
    XEvent event;
    XIEventMask evmask;
    unsigned char mask[XIMaskLen(XI_LASTEVENT)];
    XIGrabModifiers modifiers;

    win = XCreateSimpleWindow(mt->dpy, mt->root, 0, 0, mt->width, mt->height,
                              0, 0, WhitePixel(mt->dpy, mt->screen_no));
    mt->win = win;

    subwin = XCreateSimpleWindow(mt->dpy, win, 0, 0, 50, mt->height/4, 0, 0,
                                 BlackPixel(mt->dpy, mt->screen_no));
    mt->blackbar = subwin;

    subwin = XCreateSimpleWindow(mt->dpy, win, 0, mt->height/4, 50, mt->height/4, 0, 0,
                                 WhitePixel(mt->dpy, mt->screen_no));
    mt->whitebar = subwin;

    subwin = XCreateSimpleWindow(mt->dpy, win, 0, mt->height/2, 50, mt->height/4, 0, 0,
                                 BlackPixel(mt->dpy, mt->screen_no));
    mt->blackbar_os = subwin;

    subwin = XCreateSimpleWindow(mt->dpy, win, 0, 3 * mt->height/4, 50, mt->height/4, 0, 0,
                                 WhitePixel(mt->dpy, mt->screen_no));
    mt->whitebar_os = subwin;

    /* select for touch events on main window */
    XSelectInput(mt->dpy, win, ExposureMask);
    XMapSubwindows(mt->dpy, win);
    XMapWindow(mt->dpy, win);
    XMaskEvent(mt->dpy, ExposureMask, &event);

    evmask.mask = mask;
    evmask.mask_len = sizeof(mask);
    memset(mask, 0, sizeof(mask));
    evmask.deviceid = XIAllMasterDevices;

    XISetMask(mask, XI_TouchBegin);
    XISetMask(mask, XI_TouchUpdate);
    XISetMask(mask, XI_TouchEnd);
    if (ownership)
        XISetMask(mask, XI_TouchOwnership);

    XISelectEvents(mt->dpy, win, &evmask, 1);
    XIClearMask(mask, XI_TouchOwnership);

    /* grab touch events on blackbar */
    modifiers.modifiers = XIAnyModifier;
    if (XIGrabTouchBegin(mt->dpy, XIAllMasterDevices, mt->blackbar,
                         XINoOwnerEvents, &evmask, 1, &modifiers) != 0)
        error("Failed to establish passive grab on blackbar\n");

    /* grab touch events on whitebar */
    if (XIGrabTouchBegin(mt->dpy, XIAllMasterDevices, mt->whitebar,
                         XINoOwnerEvents, &evmask, 1, &modifiers) != 0)
        error("Failed to establish passive grab on whitebar\n");

    XISetMask(mask, XI_TouchOwnership);
    /* grab touch events on blackbar_os with ownership */
    if (XIGrabTouchBegin(mt->dpy, XIAllMasterDevices, mt->blackbar_os,
                         XINoOwnerEvents, &evmask, 1, &modifiers) != 0)
        error("Failed to establish passive grab on blackbar_os\n");

    /* grab touch events on whitebar_os with ownership */
    if (XIGrabTouchBegin(mt->dpy, XIAllMasterDevices, mt->whitebar_os,
                         XINoOwnerEvents, &evmask, 1, &modifiers) != 0)
        error("Failed to establish passive grab on whitebar_os\n");

    XSync(mt->dpy, False);
}

static Pixmap init_pixmap(struct multitouch *mt)
{
    Pixmap p;
    p = XCreatePixmap(mt->dpy, mt->win, mt->width, mt->height,
                      DefaultDepth(mt->dpy, mt->screen_no));
    return p;
}

static GC init_gc(struct multitouch *mt)
{
    GC gc;

    gc = XCreateGC(mt->dpy, mt->win, 0, NULL);

    return gc;
}

static int init_x11(struct multitouch *mt, int width, int height, Bool ownership)
{
    Display *dpy;
    int major = 2, minor = 2;
    int xi_opcode, xi_error, xi_event;

    dpy = XOpenDisplay(NULL);
    if (!dpy)
        return error("Invalid DISPLAY.\n");

    if (!XQueryExtension(dpy, INAME, &xi_opcode, &xi_event, &xi_error))
        return error("No X Input extension\n");

    if (XIQueryVersion(dpy, &major, &minor) != Success ||
        major * 10 + minor < 22)
        return error("Need XI 2.2\n");

    mt->dpy       = dpy;
    mt->screen_no = DefaultScreen(dpy);
    mt->screen    = DefaultScreenOfDisplay(dpy);
    mt->root      = DefaultRootWindow(dpy);
    mt->visual    = DefaultVisual(dpy, mt->screen_no);
    mt->xi_opcode = xi_opcode;
    mt->width     = width;
    mt->height    = height;

    init_windows(mt, ownership);

    mt->pixmap = init_pixmap(mt);
    mt->gc = init_gc(mt);
    if (!mt->win)
        return error("Failed to create window.\n");

    XFlush(mt->dpy);
    return EXIT_SUCCESS;
}

static void teardown(struct multitouch *mt)
{
    if (mt->win)
        XUnmapWindow(mt->dpy, mt->win);
    XCloseDisplay(mt->dpy);
}

static void print_event(struct multitouch *mt, XIDeviceEvent* event)
{
    const char *type;
    const char *windowname;
    switch(event->evtype)
    {
        case XI_TouchBegin:  type = "TouchBegin";  break;
        case XI_TouchUpdate: type = "TouchUpdate"; break;
        case XI_TouchEnd:    type = "TouchEnd";    break;
        case XI_TouchOwnership: type = "TouchOwnership"; break;
    }
    msg("Event: %s (%d)\n", type, event->deviceid);
    msg("\t%.2f/%.2f (%.2f/%.2f)\n", event->event_x, event->event_y, event->root_x, event->root_y);
    msg("\ttouchid: %d\n", event->detail);

    if (event->event == mt->blackbar)
        windowname = "blackbar";
    else if (event->event == mt->whitebar)
        windowname = "whitebar";
    else if (event->event == mt->whitebar_os)
        windowname = "whitebar_os";
    else if (event->event == mt->blackbar_os)
        windowname = "blackbar_os";
    else if (event->event == mt->win)
        windowname = "window";
    else
    {
        error("Invalid window %#lx\n", event->event);
        windowname = "UNKNOWN WINDOW";
    }

    msg("\ton %s\n", windowname);
    if (event->flags & XITouchPendingEnd)
        msg("\tflags: pending end\n");
}

static void paint_touch_begin(struct multitouch *mt, XIDeviceEvent *event)
{
    int i;
    int radius = 30;
    struct touchpoint *t = NULL;

    for (i = 0; i < mt->ntouches; i++)
    {
        if (mt->touches[i].state == TSTATE_END)
        {
            t = &mt->touches[i];
            break;
        }
    }
    if (!t)
    {
        error("Too many touchpoints, skipping\n");
        return;
    }

    t->touchid = event->detail;
    t->x = event->event_x;
    t->y = event->event_y;
    t->state = TSTATE_BEGIN;
    t->owned = !mt->needs_ownership;

    cairo_save(mt->cr);
    cairo_set_source_rgba(mt->cr, 0, 0, 0, t->owned ? 1 : .5);
    cairo_arc(mt->cr, t->x, t->y, radius, 0, 2 * M_PI);
    cairo_stroke(mt->cr);
    cairo_restore(mt->cr);
    expose(mt, t->x - radius, t->y - radius, t->x + radius, t->y + radius);
}

static struct touchpoint* find_touch(struct multitouch *mt, uint32_t touchid)
{
    int i;

    for (i = 0; i < mt->ntouches; i++)
        if (mt->touches[i].state != TSTATE_END &&
            mt->touches[i].touchid == touchid)
            return &mt->touches[i];

    return NULL;
}

static void paint_touch_update(struct multitouch *mt, XIDeviceEvent *event)
{
    struct touchpoint *t = find_touch(mt, event->detail);

    cairo_save(mt->cr);
    cairo_set_source_rgba(mt->cr, 0, 0, 0, t->owned ? 1 : .5);
    cairo_move_to(mt->cr, t->x, t->y);
    cairo_line_to(mt->cr, event->event_x, event->event_y);
    cairo_stroke(mt->cr);
    cairo_restore(mt->cr);
    expose(mt, t->x, t->y, event->event_x, event->event_y);

    t->x = event->event_x;
    t->y = event->event_y;
    t->state = TSTATE_UPDATE;
}

static void paint_touch_end(struct multitouch *mt, XIDeviceEvent *event)
{
    int rsize = 30;
    struct touchpoint *t = find_touch(mt, event->detail);

    t->x = event->event_x;
    t->y = event->event_y;
    t->state = TSTATE_END;

    cairo_save(mt->cr);
    cairo_set_source_rgba(mt->cr, 0, 0, 0, t->owned ? 1 : .5);
    cairo_rectangle(mt->cr, t->x - rsize/2, t->y - rsize/2, rsize, rsize);
    cairo_stroke(mt->cr);
    cairo_restore(mt->cr);
    expose(mt, t->x - rsize/2, t->y - rsize/2, t->x + rsize/2, t->y + rsize/2);

}

static void paint_grabs(struct multitouch *mt)
{
    const int radius = 50;
    struct grabpoint *grab;
    int i;
    double r, g, b;
    int offset;

    /* clear the whole area */
    cairo_save(mt->cr_grabs);
    cairo_save(mt->cr_grabs);
    cairo_set_operator(mt->cr_grabs, CAIRO_OPERATOR_CLEAR);
    cairo_paint(mt->cr_grabs);
    cairo_restore(mt->cr_grabs);

    for (i = 0; i < mt->ngrabs; i++)
    {
        grab = &mt->grabs[i];

        if (grab->state == TSTATE_END)
            continue;

        if (grab->grab_window == mt->blackbar)
        {
            r = 0; g = 0; b = 1;
            offset = 0;
        } else if (grab->grab_window == mt->whitebar)
        {
            r = 1; g = 0; b = 0;
            offset = mt->height/4;
        } else if (grab->grab_window == mt->blackbar_os)
        {
            r = 0; g = 1; b = 0;
            offset = mt->height/2;
        } else
        {
            r = 0; g = 1; b = 1;
            offset = 3 * mt->height/4;
        }

        cairo_save(mt->cr_grabs);

        /* draw starting circle */
        cairo_set_source_rgba(mt->cr_grabs, r, g, b, 1);
        cairo_move_to(mt->cr_grabs, grab->startx + radius, offset + grab->starty);
        cairo_arc(mt->cr_grabs, grab->startx, offset + grab->starty, radius, 0, 2 * M_PI);
        cairo_stroke(mt->cr_grabs);

        /* draw line to current point */
        cairo_set_source_rgba(mt->cr_grabs, r, g, b, 1);
        cairo_move_to(mt->cr_grabs, grab->startx, offset + grab->starty);
        cairo_line_to(mt->cr_grabs, grab->x, offset + grab->y);

        cairo_stroke(mt->cr_grabs);
        cairo_restore(mt->cr_grabs);

        msg("%d: %.2f/%.2f %.2f/%.2f\n", i, grab->startx, grab->starty, grab->x,
                grab->y);
    }

    cairo_restore(mt->cr_grabs);
}

static void handle_grabbed_event(struct multitouch *mt, XIDeviceEvent *event)
{
    struct grabpoint *grab = NULL;

    int i;
    for (i = 0; i < mt->ngrabs; i++)
    {
        grab = &mt->grabs[i];
        if (event->evtype == XI_TouchBegin && grab->state == TSTATE_END)
        {
                grab->state = TSTATE_BEGIN;
                grab->touchid = event->detail;
                grab->startx = event->event_x;
                grab->starty = event->event_y;
                grab->grab_window = event->event;
                grab->nevents = 0;
                break;
        } else if (event->evtype != XI_TouchBegin) {
            if (grab->state != TSTATE_END && grab->touchid == event->detail)
                break;
        }
    }
    if (!grab) {
        error("oops, touchpoint for %d (touchid %d) not found\n",
                event->evtype, event->detail);
        return;
    }

    grab->x = event->event_x;
    grab->y = event->event_y;
    grab->nevents++;

    paint_grabs(mt);
    expose(mt, 0, 0, mt->width, mt->height);

    if (grab->state != TSTATE_END &&
        (event->evtype == XI_TouchEnd || (grab->nevents > 100 && event->evtype != XI_TouchOwnership)))
    {
        int mode = XIAcceptTouch;
        const char *modestr = "Accept";
        if (event->event == mt->whitebar || event->event == mt->whitebar_os)
        {
            mode = XIRejectTouch;
            modestr = "Reject";
        }
        grab->state = TSTATE_END;
        msg("%s touch %d on %#lx.\n", modestr, event->detail, event->event);
        XIAllowTouchEvents(mt->dpy, event->deviceid, event->detail,
                event->event, mode);
    } else if (event->evtype == XI_TouchUpdate)
            grab->state = TSTATE_UPDATE;
}

static void handle_ownership(struct multitouch *mt, XITouchOwnershipEvent *event)
{
    const int radius = 10;
    struct touchpoint *t = find_touch(mt, event->touchid);
    t->owned = True;

    cairo_save(mt->cr);
    cairo_set_source_rgba(mt->cr, 1, 0, 0, t->owned ? 1 : .5);
    cairo_arc(mt->cr, t->x, t->y, radius, 0, 2 * M_PI);
    cairo_stroke(mt->cr);
    cairo_restore(mt->cr);
    expose(mt, t->x - radius/2, t->y - radius/2, t->x + radius/2, t->y + radius/2);
}

static void paint_event(struct multitouch *mt, XIDeviceEvent *event)
{
    if (event->event == mt->blackbar || event->event == mt->whitebar ||
        event->event == mt->blackbar_os || event->event == mt->whitebar_os)
    {
        handle_grabbed_event(mt, event);
        return;
    }

    switch(event->evtype)
    {
        case XI_TouchBegin: paint_touch_begin(mt, event); break;
        case XI_TouchUpdate: paint_touch_update(mt, event); break;
        case XI_TouchEnd: paint_touch_end(mt, event); break;
        case XI_TouchOwnership: handle_ownership(mt, (XITouchOwnershipEvent*)event); break;
    }
}


static int init_cairo(struct multitouch *mt)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    /* frontbuffer */
    surface = cairo_xlib_surface_create(mt->dpy, mt->win,
                                        mt->visual, mt->width, mt->height);
    if (!surface)
        return error("Failed to create cairo surface\n");

    mt->surface_win = surface;

    cr = cairo_create(surface);
    if (!cr)
        return error("Failed to create cairo context\n");

    mt->cr_win = cr;

    /* grab drawing backbuffer */
    surface = cairo_surface_create_similar(surface,
                                           CAIRO_CONTENT_COLOR_ALPHA,
                                           mt->width, mt->height);
    if (!surface)
        return error("Failed to create cairo surface\n");

    mt->surface_grabs = surface;

    cr = cairo_create(surface);
    if (!cr)
        return error("Failed to create cairo context\n");

    mt->cr_grabs = cr;

    /* backbuffer */
    surface = cairo_surface_create_similar(surface,
                                           CAIRO_CONTENT_COLOR_ALPHA,
                                           mt->width, mt->height);
    if (!surface)
        return error("Failed to create cairo surface\n");

    mt->surface = surface;

    cr = cairo_create(surface);
    if (!cr)
        return error("Failed to create cairo context\n");

    cairo_set_line_width(cr, 1);
    cairo_set_source_rgb(cr, .85, .85, .85);
    cairo_rectangle(cr, 0, 0, mt->width, mt->height);
    cairo_fill(cr);

    mt->cr = cr;

    expose(mt, 0, 0, mt->width, mt->height);

    return EXIT_SUCCESS;
}

static void expose(struct multitouch *mt, int x1, int y1, int x2, int y2)
{
    cairo_set_source_surface(mt->cr_win, mt->surface, 0, 0);
    cairo_paint(mt->cr_win);

    cairo_save(mt->cr_win);
    cairo_set_source_surface(mt->cr_win, mt->surface_grabs, 0, 0);
    cairo_mask_surface(mt->cr_win, mt->surface_grabs, 0, 0);
    cairo_restore(mt->cr_win);

}

static int main_loop(struct multitouch *mt)
{
    struct pollfd fd;

    fd.fd = ConnectionNumber(mt->dpy);
    fd.events = POLLIN;

    while (running)
    {

        if (poll(&fd, 1, 500) <= 0)
            continue;

        while (XPending(mt->dpy)) {
            XEvent ev;
            XGenericEventCookie *cookie = &ev.xcookie;

            XNextEvent(mt->dpy, &ev);
            if (ev.type == Expose) {
                expose(mt, ev.xexpose.x, ev.xexpose.y,
                           ev.xexpose.x + ev.xexpose.width,
                           ev.xexpose.y + ev.xexpose.height);
            } else if (ev.type == GenericEvent &&
                XGetEventData(mt->dpy, cookie) &&
                cookie->type == GenericEvent &&
                cookie->extension == mt->xi_opcode)
            {
                print_event(mt, (XIDeviceEvent* )cookie->data);
                paint_event(mt, (XIDeviceEvent* )cookie->data);
            }

            XFreeEventData(mt->dpy, cookie);
        }
    }

    return EXIT_SUCCESS;
}

static void init(struct multitouch *mt, Bool ownership)
{
    memset(mt, 0, sizeof(*mt));

    mt->ntouches = MAX_TOUCHES;
    mt->ngrabs = MAX_TOUCHES;
    mt->needs_ownership = ownership;
}

int main_multitouch(int argc, char **argv)
{
    int rc;
    struct multitouch mt;
    Bool ownership = False;

    usage();

    if (argc > 1 && strcmp(argv[1], "--with-ownership") == 0)
        ownership = True;

    init(&mt, ownership);

    rc = init_x11(&mt, 800, 600, ownership);
    if (rc != EXIT_SUCCESS)
        return rc;

    rc = init_cairo(&mt);
    if (rc != EXIT_SUCCESS)
        return rc;

    signal(SIGINT, sighandler);

    main_loop(&mt);

    teardown(&mt);

    return EXIT_SUCCESS;
}
