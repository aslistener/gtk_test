#include "x11_test.h"



   Display *d;
   Window w;

void ChildWindowRealizeCallback(GtkWidget *window, 
      GdkEvent *event, gpointer data) {

    GdkDisplay* gdkDisplay = gdk_window_get_display(window->window);
    Display* xDisplay = gdk_x11_display_get_xdisplay(gdkDisplay);
 
    //LOG(XConnectionNumber(d));
    PRINT_VAL(XConnectionNumber(xDisplay));
    PRINT_VAL(GDK_WINDOW_XID(gtk_widget_get_window(window)));

    XReparentWindow(xDisplay, GDK_WINDOW_XID(gtk_widget_get_window(window)), w, 400, 400);
}

void PaintChild(){


}


GdkDrawable* createPixmap(){
    GdkVisual* sys_visual = gdk_visual_get_system();
    // GdkDrawable* pixmap2 = gdk_pixmap_new(NULL,  pixmap_draw_rect.width(), pixmap_draw_rect.height(),  sys_visual->depth);
    GdkColormap* c = gdk_colormap_new(gdk_visual_get_system(), FALSE);
    // GdkPixbuf *ret = NULL;
    // Drawable drawable = gdk_x11_drawable_get_xid(pixmap2);
    Colormap cmap = gdk_x11_colormap_get_xcolormap(c);
    Visual* visual = gdk_x11_visual_get_xvisual(sys_visual);

     GdkDrawable* pixmap2 = gdk_pixmap_new(NULL,  400, 400,  sys_visual->depth);
     Pixmap pixmap = gdk_x11_drawable_get_xid(pixmap2);

    // Pixmap pixmap  = XCreatePixmap(d, DefaultScreen(d), 400, 400, 1);
    //Pixmap pixmap  = XCreatePixmapFromBitmapData(d, DefaultScreen(d), 400, 400, 1);


//    cairo_surface_t *surface 
//                = cairo_xlib_surface_create(d, pixmap, visual, 400, 400);
    cairo_t* dst = gdk_cairo_create(pixmap2);
        //=  cairo_create(surface);
    cairo_set_source_rgb(dst, 1.0, 1.0, 0.0);
    cairo_set_operator (dst, CAIRO_OPERATOR_SOURCE);
    cairo_rectangle(dst, 0, 0, 400, 400);
    cairo_clip(dst);
    cairo_paint(dst);

    // cairo_destroy(dst);
    gdk_pixbuf_xlib_init(d, 0);

    GdkPixbuf * pixbuf = gdk_pixbuf_xlib_get_from_drawable(NULL,  pixmap,
        cmap, visual, 0, 0, 0, 0, 400, 400);
        gdk_pixbuf_save(pixbuf, "/code/tmppngs/windowless_paint.png",  "png",  NULL, "quality", "100", NULL);
    
    return pixmap2;
}

int TestOpenDisplay() {
   int s;
   XEvent e;

                        /* open connection with the server */
   d=XOpenDisplay(NULL);
   if(d==NULL) {
     printf("Cannot open display\n");
     exit(1);
   }
   s=DefaultScreen(d);

    Colormap colormap = DefaultColormap(d, s);

    XColor blue;
    XAllocNamedColor(d, colormap, "blue", &blue, &blue);


    // Initialize container window attributes
    XSetWindowAttributes attrs;

    attrs.event_mask
        = SubstructureRedirectMask // handle child window requests      (MapRequest)
        | SubstructureNotifyMask   // handle child window notifications (DestroyNotify)
        | StructureNotifyMask      // handle container notifications    (ConfigureNotify)
        | ExposureMask             // handle container redraw           (Expose)
        | VisibilityChangeMask ;

    attrs.do_not_propagate_mask = 0; // do not hide any events from child window

    attrs.background_pixel = blue.pixel; // background color

    unsigned long attrs_mask = CWEventMask  // enable attrs.event_mask
                             | NoEventMask  // enable attrs.do_not_propagate_mask
                             | CWBackPixel  // enable attrs.background_pixel
                        ;

                        /* create window */
  w=XCreateWindow(d, RootWindow(d, s),
            100,
            100,
            1000,
            1000,
            1,
            CopyFromParent,
            InputOutput,
            CopyFromParent,
            attrs_mask,
            &attrs);

   // Process Window Close Event through event handler so XNextEvent does Not fail
   Atom delWindow = XInternAtom( d, "WM_DELETE_WINDOW", 0 );
   XSetWMProtocols(d , w, &delWindow, 1);

                        /* select kind of events we are interested in */
   //XSelectInput(d, w, ExposureMask | KeyPressMask);

                        /* map (show) the window */
   XMapWindow(d, w);

   //  gtk_widget_get_window(button);
     
    //  GdkWindow* widget_frame = gdk_window_foreign_new(w);
    //  gtk_widget_set_parent_window(button, widget_frame);


  std::thread reparent_thread([&](){
    std::this_thread::sleep_for(5s);
    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    // gtk_window_set_title (GTK_WINDOW (window), "Hello World");
//     GtkWidget * button = gtk_button_new_with_label("Button");
//     gtk_widget_show(button);
//     gtk_container_add(GTK_CONTAINER(window), button);

   gtk_widget_set_app_paintable(window, TRUE);
    g_signal_connect(G_OBJECT(window), "realize",
          G_CALLBACK(ChildWindowRealizeCallback), (gpointer)(&w));

    //gtk_widget_realize(button);
    gtk_widget_show_all(window);
    gtk_main();
  });
  reparent_thread.detach();

    Display* child_display = NULL;
    Window child_window = 0;

    Atom wm_delete = XInternAtom(d, "WM_DELETE_WINDOW", True);
    // Subscribe WM_DELETE_WINDOW message
    XSetWMProtocols(d, w, &wm_delete, 1);

    // XSelectInput(d, child_window, ExposureMask);

    //GC xgc = XCreateGC();

    GdkDrawable* pixmap = createPixmap();
    GC gc;

    //     gtk_widget_show (button);
    
    /* event loop */
    for (;;) {
        XEvent event;
        XNextEvent(d, &event);

        PRINT_VAL(event_names[event.type]);

        // Map child window when it requests and store its display and window id
        if (event.type == MapRequest) {
            XMapWindow(event.xmaprequest.display, event.xmaprequest.window);

            child_display = event.xmaprequest.display;
            child_window = event.xmaprequest.window;

            PRINT_VAL(child_window);
            PRINT_VAL(w);

            if (child_window) {
                // Get container window attributes
                XWindowAttributes attrs;
                XGetWindowAttributes(d, w, &attrs);

                // Move and resize child
                XMoveResizeWindow(child_display,
                                  child_window,
                                  100, 100, attrs.width - 200, attrs.height - 200);
                




                XSelectInput(d, child_window, ExposureMask);


            }
        }
        // Propagate resize event to child window, and also resize it after MapRequest
        else if (event.type == ConfigureNotify) {
            if (child_window) {
                // Get container window attributes
                XWindowAttributes attrs;
                XGetWindowAttributes(d, w, &attrs);

                // Move and resize child
                XMoveResizeWindow(child_display,
                                  child_window,
                                  100, 100, attrs.width - 200, attrs.height - 200);
                gc = XCreateGC(d, child_window, 0, 0);
                XSetBackground (d, gc, BlackPixel (child_display, DefaultScreen(d))); 

                EnumerateWindows(d, w, 1, 1, NULL);

            }
            
        }

        // Refresh container window
        else if (event.type == Expose) {
            PRINT_VAL(event.xexpose.window);

            if(event.xexpose.window == child_window) {
                XDrawRectangle(d, child_window, gc, 100, 100, 200, 200);
            }
            // if(event.xexpose.window == child_window) {
            //      cairo_surface_t *surface 
            //      = cairo_xlib_surface_create(d, child_window, CopyFromParent, 400, 400);
            //  cairo_t* dst =  cairo_create(surface);

        
            // cairo_t *src = gdk_cairo_create(pixmap);
            // cairo_set_source_surface(dst, cairo_get_target(src), 0, 0);

            // //cairo_set_source_rgb(dst, 1.0, 1.0, 0.0);
            // cairo_set_operator (dst, CAIRO_OPERATOR_SOURCE);
            // cairo_paint(dst);
            // cairo_destroy(dst);
            // //XSync(d, 0);
            // }
        }
        else if (event.type == MapNotify) {
            PRINT_VAL(event.xmap.window);

            // if(event.xmap.window == child_window){
            //     XClearWindow(child_display, child_window);   

            // }
            
        }
        else if (event.type == VisibilityNotify) {
            PRINT_VAL(event.xvisibility.window);
        }

        // Exit if child window was destroyed
        else if (event.type == DestroyNotify) {
            fprintf(stderr, "child window destroyed, exiting\n");
            break;
        }

        // Close button
        else if (event.type == ClientMessage) {
            if (event.xclient.data.l[0] == wm_delete) {
                break;
            }
        }
    }
                        /* destroy our window */
   XDestroyWindow(d, w);

                        /* close connection to server */
   XCloseDisplay(d);

   return 0;
 }

//  void clipWindow(Display display, Window win)
//  {
//     // Display dpy; //This is your display, we'll assume it is a valid Display
//     // GC gc; //This is your GC, we'll assume it is a valid GC
//     // XRectangle recs[1] = {XRectangle(20, 20, 100, 100) }; //This is an array containing the clipping regions you want.
//     // int recs_n = 1; //This is the number of rectangles in the 'recs' array.

//     // Pixmap bitmap = XCreateBitmap(display, DefaultScreen(display), 100, 100, 1);
//     // GC gc=XCreateGC (dpy, clip_mask, 0, NULL);

//     // XSetClipRectangles(display, gc, 0, 0, recs, recs_n, Unsorted); //Enable clipping
//     // drawMyClippedGraphics(); //Call to whatever you want to use for drawing
//     // XSetClipMask(dpy, gc, None); //Restore the GC
//  }

 void test_clip()
 {
  Display *dpy;
  Window root;
  XWindowAttributes wa;
  GC g;
  GC gc;

  XColor redx, reds;

  int x=10, y=10; 	/* position of the ball */
  int dx=1, dy=1;	/* direction of the ball */

  Pixmap double_buffer;
  Pixmap clip_mask;


  /* open the display (connect to the X server) */
  dpy = XOpenDisplay (NULL);


  /* get the root window */
  root = DefaultRootWindow (dpy);


  /* get attributes of the root window */
  XGetWindowAttributes(dpy, root, &wa);


  /* create a GC for drawing in the window */
  g = XCreateGC (dpy, root, 0, NULL);


  /* create the double buffer */
  double_buffer = XCreatePixmap(dpy, root,
                  wa.width, wa.height, wa.depth);
  XSetForeground(dpy, g, BlackPixelOfScreen(DefaultScreenOfDisplay(dpy)));
  XFillRectangle(dpy, double_buffer, g, 0, 0, wa.width, wa.height);
  XCopyArea(dpy, double_buffer, root, g, 0, 0, wa.width, wa.height, 0, 0);


  /* create the clipping mask */
  clip_mask = XCreatePixmap(dpy, double_buffer, wa.width, wa.height, 1);
  gc=XCreateGC (dpy, clip_mask, 0, NULL);


  /* allocate the red color */
  XAllocNamedColor(dpy,
                     DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy)),
                     "red",
                     &reds, &redx);


  /* draw something */
  while (1)
    {
      /* clear clipping mask */
      XSetBackground(dpy, gc, 0);
      XFillRectangle(dpy, clip_mask, gc, 0, 0, wa.width, wa.height);


      /* remove the ball from the screen */
      XSetForeground(dpy, g, BlackPixelOfScreen(DefaultScreenOfDisplay(dpy)));
      XFillArc(dpy, double_buffer, g, x, y, 40, 40, 0, 360*64);


      /* region changed, set mask */
      XSetForeground(dpy, gc, 1);
      XFillArc(dpy, clip_mask, gc, x, y, 40, 40, 0, 360*64);


      /* change position */
      x+=dx;
      y+=dy;


      /* draw in the new position */
      XSetForeground(dpy, g, reds.pixel);
      XFillArc(dpy, double_buffer, g, x, y, 40, 40, 0, 360*64);


      /* region changed, set mask */
      XSetForeground(dpy, gc, 1);
      XFillArc(dpy, clip_mask, gc, x, y, 40, 40, 0, 360*64);


      /* bounce (if the ball is at the edge of the screen) */
      if( x<=0 || x>=wa.width-40 )
        dx= -dx;
      if( y<=0 || y>=wa.height-40 )
        dy= -dy;


      /* set clipping mask */
      XSetClipMask(dpy, g, clip_mask);


      /* copy the buffer to the window */
      XCopyArea(dpy, double_buffer, root, g, 0, 0, wa.width, wa.height, 0, 0);


      /* flush changes and sleep */
      XFlush(dpy);
      usleep (10);
    }


  XCloseDisplay (dpy);

 }

# define arrow_width 16
# define arrow_height 16

static unsigned char arrow_bits [ ] = {
0x00 , 0x00 , 0x06 , 0x00 , 0x1e , 0x00 , 0x7c , 0x00 , 0xfc , 0x01 , 0xf8 , 0x07 ,
0xf8 , 0x1f , 0xf8 , 0x7f , 0xf0 , 0x7f , 0xf0 , 0x03 , 0xe0 , 0x07 , 0xe0 , 0x06 ,
0xc0 , 0x0c , 0xc0 , 0x18 , 0x80 , 0x30 , 0x00 , 0x00 } ;

# define mask_width 16
# define mask_height 16
static unsigned char mask_bits [ ] = {
0x07 , 0x00 , 0x1f , 0x00 , 0x7f ,0x00 , 0xf6 , 0x01 , 0xc6 , 0x07 , 0x8e , 0x1f,
0x0c , 0x3e , 0x1c , 0xfc , 0x38 ,0xfc , 0x38 , 0xfc , 0x78 , 0x0f , 0xf0 , 0x1f ,
0xf0 , 0x3f , 0xe0 , 0x7d , 0xe0 ,0x79 , 0xc0 , 0x71
};

int test_double_buffer_clip ()
{
  Display *mydisplay ;
  Window baseWindow ;
  XSetWindowAttributes myat ;
  XSizeHints wmsize ;
  XWMHints wmhints ;
  XTextProperty windowName , iconName ;
  XEvent baseEvent ;
  XColor exact , closest;
  GC mygc ;
  XGCValues myGCValues ;
  Pixmap pattern , mask ;
  char *window_name = "Transparent" ;
  char * icon_name = "Tr" ;
  int screen_num , done ;
  unsigned long mymask ;
  int x, y;
  
  mydisplay = XOpenDisplay (NULL) ;
  
  screen_num = DefaultScreen ( mydisplay ) ;
  myat.border_pixel = BlackPixel ( mydisplay , screen_num ) ;
  XAllocNamedColor (mydisplay , XDefaultColormap ( mydisplay , screen_num ) ,
                    "red" , &exact , &closest ) ;
  
  myat.background_pixel = closest.pixel;
  myat.event_mask = ButtonPressMask | ExposureMask ;
  mymask = CWBackPixel | CWBorderPixel | CWEventMask ;
  baseWindow = XCreateWindow ( mydisplay , RootWindow ( mydisplay , screen_num ) ,
          300 , 300 , 350 , 400 , 3 ,
          DefaultDepth (mydisplay, screen_num), InputOutput ,
          DefaultVisual(mydisplay, screen_num), mymask , &myat ) ;
  
  wmsize.flags = USPosition | USSize ;
  XSetWMNormalHints ( mydisplay , baseWindow , &wmsize ) ;
  wmhints.initial_state = NormalState ;
  wmhints.flags = StateHint ;
  XSetWMHints ( mydisplay , baseWindow , &wmhints ) ;
  
  XStringListToTextProperty (&window_name , 1 , &windowName ) ;
  XSetWMName( mydisplay , baseWindow , &windowName ) ;
  XStringListToTextProperty (&icon_name , 1 , &iconName ) ;
  XSetWMIconName ( mydisplay , baseWindow , &iconName ) ;
  
  pattern = XCreatePixmapFromBitmapData ( mydisplay , baseWindow ,
  (char *)arrow_bits , arrow_width , arrow_height ,
  WhitePixel ( mydisplay , screen_num ) ,
  BlackPixel ( mydisplay , screen_num ) ,
  DefaultDepth ( mydisplay , screen_num ) ) ;
  
  mask = XCreatePixmapFromBitmapData ( mydisplay , baseWindow ,
  (char *)mask_bits , mask_width , mask_height , 1 , 0 , 1 ) ;

  mymask = GCForeground | GCBackground | GCClipMask ;
  myGCValues.background = WhitePixel ( mydisplay , screen_num ) ;
  myGCValues.foreground = BlackPixel ( mydisplay , screen_num ) ;
  myGCValues.clip_mask = mask ;
  
  mygc = XCreateGC ( mydisplay , baseWindow , mymask , &myGCValues ) ;
  
  XMapWindow( mydisplay , baseWindow ) ;
  
  done = 0 ;
  while ( done == 0 ) {
    XNextEvent ( mydisplay , &baseEvent ) ;
    switch ( baseEvent.type ) {
    case Expose :
      break ;
    case ButtonPress :
      if ( baseEvent.xbutton.button == Button3 ) 
      {
          x = baseEvent.xbutton.x ;
          y = baseEvent.xbutton.y ;
          XSetClipOrigin( mydisplay , mygc , x , y ) ;
          XCopyPlane ( mydisplay , pattern , baseWindow , mygc , 0 , 0 ,
              arrow_width , arrow_height , x , y , 1 ) ;
      }
      break ;
    }
  }
  
  XUnmapWindow( mydisplay , baseWindow ) ;
  XDestroyWindow ( mydisplay , baseWindow ) ;
  XCloseDisplay ( mydisplay ) ;
}


/* size of the window */
#define W_WIDTH 640
#define W_HEIGHT 480

/* size of the four rectangles that will be drawn in the window */
#define R_WIDTH 80
#define R_HEIGHT 60

Display *dpy;
// Window w;

/* convenience variables */
int BLACK_PIXEL;
int WHITE_PIXEL;

/* the four rectangles that will be drawn: one in each corner of the
 * window */
XRectangle rectangles[4] =
{
    { 0, 0, R_WIDTH, R_HEIGHT },
    { 0, W_HEIGHT-R_HEIGHT, R_WIDTH, R_HEIGHT },
    { W_WIDTH-R_WIDTH, W_HEIGHT-R_HEIGHT, R_WIDTH, R_HEIGHT },
    { W_WIDTH-R_WIDTH, 0, R_WIDTH, R_HEIGHT }
};
        
int main222222(int argc, char **argv)
{
    XGCValues shape_xgcv;
    Pixmap pmap;
    GC shape_gc;
    GC gc;
    XGCValues gcv;
    int run = 1; /* loop control variable */

    /* open the display */
    if(!(dpy = XOpenDisplay(getenv("DISPLAY")))) {
        fprintf(stderr, "can't open display\n");
        return EXIT_FAILURE;
    }

    /* convenience */
    BLACK_PIXEL = BlackPixel(dpy, DefaultScreen(dpy));
    WHITE_PIXEL = WhitePixel(dpy, DefaultScreen(dpy));

    w = XCreateWindow(dpy, DefaultRootWindow(dpy), 0, 0,
            W_WIDTH, W_HEIGHT, 0, CopyFromParent,
            InputOutput, CopyFromParent, 0, NULL);
    
    /* Try to create a transparent background.
     *
     * The idea/technique attempts to mimic lines 342--360 of
     * "Eyes.c", from the "xeyes" source.  (The xeyes source is part
     * of the X11 source package.)
     *
     * Every other example I've seen uses a pixmap, but I'd like to
     * not have a pixmap as a requirement.
     */
    pmap = XCreatePixmap(dpy, w, W_WIDTH, W_HEIGHT, 1);
    shape_gc = XCreateGC(dpy, pmap, 0, &shape_xgcv);

    
    XSetForeground(dpy, shape_gc, WhitePixel(dpy, DefaultScreen(dpy)));
    // XSetBackground(dpy, shape_gc, 0);
    XFillRectangle(dpy, pmap, shape_gc, 0, 0, W_WIDTH, W_HEIGHT);

    XShapeCombineMask (dpy, w, ShapeBounding,
            0, 0, pmap, ShapeSet);

    /* If I remove everything above (until the comment), and replace
     * with the following, this application works as expected (e.g.,
     * draws a black window with white rectanles at each corner */
    /* XSetWindowBackground(dpy, w, BLACK_PIXEL); */

    /* create a graphics context for drawing */
    gcv.foreground = WHITE_PIXEL;
    gcv.line_width = 1;
    gcv.line_style = LineSolid;
    gc = XCreateGC(dpy, w,
            GCForeground | GCLineWidth | GCLineStyle, &gcv);

    /* register events: ExposureMask for re-drawing, ButtonPressMask
     * to capture mouse button press events */
    XSelectInput(dpy, w, ExposureMask | ButtonPressMask);

    XMapWindow(dpy, w);
    XSync(dpy, False);

    while(run) {
        XEvent xe;
        XNextEvent(dpy, &xe);
        switch (xe.type) {
            case Expose:
                /* whenever we get an expose, draw the rectangles */
                XSetForeground(dpy, gc, WHITE_PIXEL);
                XDrawRectangles(dpy, w, gc, rectangles, 4);
                XFillRectangles(dpy, w, gc, rectangles, 4);
                XSync(dpy, False);
                break;
            case ButtonPress: /* quit if a button is pressed */
                /* note that when using XShapeCombineMask(), i.e.
                 * trying to get a "transparent" background,
                 * no ButtonPress events are ever recognized
                 */
                printf("ButtonPress\n");
                run = 0;
                break;
            default:
                printf("Caught event %i\n", xe.type);
        }
    }

    XDestroyWindow(dpy, w);
    XCloseDisplay(dpy);

    return 0;
}