#include "x11_test.h"

class X11Test{
public:  
  X11Test(){ init(); }
  ~X11Test();
  static void handleEvents(X11Test *p_this);

private:
  void init();
  void initSubWindow();

  void handleExposeEvent(const XEvent &event);
  void handleMapRequestEvent(const XEvent &event);
  void handleMapEvent(const XEvent &event);
  void handleConfigureEvent(const XEvent &event);

  static void ChildWindowRealizeCallback(GtkWidget *window, 
      gpointer data);

  Display *display_ = 0;
  Window window_ = 0;
  Display *child_display_ = 0;
  Window child_window_ = 0;
  GC gc_ = 0;
};

  // Display *display_ = 0;
  // Window window_ = 0;
  // Display *child_display_ = 0;
  // Window child_window_ = 0;
  // GC gc_ = 0;

X11Test::~X11Test()
{
  
}

void X11Test::ChildWindowRealizeCallback(GtkWidget *window, 
      gpointer data) 
{

  X11Test * g_this = (X11Test *)data;
  GdkDisplay* gdkDisplay = gdk_window_get_display(window->window);
  Display* xDisplay = gdk_x11_display_get_xdisplay(gdkDisplay);

  PRINT_VAL(XConnectionNumber(xDisplay));
  PRINT_VAL(GDK_WINDOW_XID(gtk_widget_get_window(window)));

  XReparentWindow(xDisplay, GDK_WINDOW_XID(gtk_widget_get_window(window)), g_this->window_, 400, 400);
}

void X11Test::init()
{
  display_ = XOpenDisplay(NULL);
  if(!display_) 
  {
    printf("Cannot open display\n");
    exit(1);
  }

  int screen=DefaultScreen(display_);
  Colormap colormap = DefaultColormap(display_, screen);
  XColor blue;
  XAllocNamedColor(display_, colormap, "blue", &blue, &blue);

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
  window_=XCreateWindow(display_, RootWindow(display_, screen),
          100,100,1000,1000,
          1,CopyFromParent,InputOutput,CopyFromParent,
          attrs_mask,&attrs);

  initSubWindow();

  Atom del_window = XInternAtom(display_, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols(display_ , window_, &del_window, 1);
  
  XMapWindow(display_, window_);

}

void X11Test::initSubWindow()
{
  std::thread reparent_thread([&, this](){
    std::this_thread::sleep_for(1s);
    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
    gtk_window_set_title (GTK_WINDOW (window), "Hello World");
    GtkWidget * button = gtk_button_new_with_label("Button");
    gtk_widget_show(button);
    gtk_container_add(GTK_CONTAINER(window), button);
    gtk_widget_set_app_paintable(window, TRUE);

    g_signal_connect(G_OBJECT(window), "realize",
          G_CALLBACK(ChildWindowRealizeCallback), (gpointer)(this));
   //gtk_widget_realize(button);
    gtk_widget_show_all(window);
    gtk_main();
  });
  reparent_thread.detach();
}


void X11Test::handleEvents(X11Test *p_this)
{
  Atom wm_delete = XInternAtom(p_this->display_, "WM_DELETE_WINDOW", True);
  // Subscribe WM_DELETE_WINDOW message
  XSetWMProtocols(p_this->display_, p_this->window_, &wm_delete, 1);
  
  bool is_running = true;
  while(is_running) 
  {
    XEvent event;
    XNextEvent(p_this->display_, &event);
    PRINT_VAL(event_names[event.type]);
    PRINT_VAL(event.xmap.window);
    // Map child window when it requests and store its display and window id
    switch(event.type)
    {
      case MapRequest:
        p_this->handleMapRequestEvent(event);
      break;
      case ConfigureNotify:
      {
        p_this->handleConfigureEvent(event);
        break;
      }
      case Expose:
        p_this->handleExposeEvent(event);
      break;
      case MapNotify:
        p_this->handleMapEvent(event);
      break;
      case VisibilityNotify:
        PRINT_VAL(event.xvisibility.window);
      break;
      case DestroyNotify:
        fprintf(stderr, "child window destroyed, exiting\n");
      break;
      case ClientMessage:
      if (event.xclient.data.l[0] == wm_delete) 
      {
        is_running = false;
        break;
      }
      break;
    }
  }
  XDestroyWindow(p_this->display_, p_this->window_); /* destroy our window */
  XCloseDisplay(p_this->display_);  /* close connection to server */
}

void X11Test::handleExposeEvent(const XEvent &event)
{
  // if(child_window_)
  //   XDrawRectangle(display_, child_window_, gc_, 100, 100, 200, 200);
  if(event.xexpose.window == child_window_) 
  {
      XDrawRectangle(display_, child_window_, gc_, 100, 100, 200, 200);
  }
  // if(event.xexpose.window == child_window) {
  //      cairo_surface_t *surface 
  //      = cairo_xlib_surface_create(d, child_window, CopyFromParent, 400, 400);
  //  cairo_t* dst =  cairo_create(surfa
  
  // cairo_t *src = gdk_cairo_create(pixmap);
  // cairo_set_source_surface(dst, cairo_get_target(src), 0,
  // //cairo_set_source_rgb(dst, 1.0, 1.0, 0.0);
  // cairo_set_operator (dst, CAIRO_OPERATOR_SOURCE);
  // cairo_paint(dst);
  // cairo_destroy(dst);
  // //XSync(d, 0);
  // }
}

void X11Test::handleMapRequestEvent(const XEvent &event)
{
  XMapWindow(event.xmaprequest.display, event.xmaprequest.window);
  child_display_ = event.xmaprequest.display;
  child_window_ = event.xmaprequest.window;

  PRINT_VAL(child_window_);
  PRINT_VAL(window_);

  if (child_window_) 
  {
    // Get container window attributes
    XWindowAttributes attrs;
    XGetWindowAttributes(display_, window_, &attrs);
    // Move and resize child
    XMoveResizeWindow(child_display_,
                      child_window_,
                      100, 100, attrs.width - 200, attrs.height - 200);

    XSelectInput(display_, child_window_, ExposureMask);

    std::vector<unsigned long> window_ids;
    EnumerateWindows(child_display_, child_window_, 1, 1, &window_ids);
    std::for_each(window_ids.begin(), window_ids.end(), [=](unsigned long id){
      XSelectInput(display_, id, ExposureMask);
    });

  }
}

void X11Test::handleMapEvent(const XEvent &event)
{
  
}

void X11Test::handleConfigureEvent(const XEvent &event)
{
  if (child_window_)
  {
    // Get container window attributes
    XWindowAttributes attrs;
    XGetWindowAttributes(display_, window_, &attrs);
    // Move and resize child
    XMoveResizeWindow(child_display_,
                      child_window_,
                      100, 100, attrs.width - 200, attrs.height - 200);
    gc_ = XCreateGC(child_display_, child_window_, 0, 0);
    XSetBackground (display_, gc_, BlackPixel (display_, DefaultScreen(display_)));
  }
}

void testClip()
{
  X11Test x11_test;
  X11Test::handleEvents(&x11_test) ;
}