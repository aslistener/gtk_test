#include "x11_test.h"

class X11Test{
public:  
  X11Test();
  ~X11Test();
  static void handleEvents(X11Test *p_this);

public:
  void queryShapeExtension();

  void createPixmap();

private:
  void init();
  void initSubWindow();


  void handleExposeEvent(const XEvent &event);
  void handleMapRequestEvent(const XEvent &event);
  void handleMapEvent(const XEvent &event);
  void handleConfigureEvent(const XEvent &event);

  static void ChildWindowRealizeCallback(GtkWidget *window, 
      gpointer data);

  static void ClipButtonClickRealizeCallback(GtkWidget *window, 
      GdkEvent  *event,
      gpointer data);    

  Display *display_ = 0;
  Window window_ = 0;
  Display *child_display_ = 0;
  Window child_window_ = 0;
  GtkWidget *gtk_child_window_ = 0;
  GC gc_ = 0;

  std::vector<XRectangle> rectangles;
  unsigned char* mask_data_ = 0;
  int window_width_;
  int window_height_;
  Pixmap pixmap_ = 0;
};

  // Display *display_ = 0;
  // Window window_ = 0;
  // Display *child_display_ = 0;
  // Window child_window_ = 0;
  // GC gc_ = 0;

X11Test::X11Test()
  :window_width_(1000),
  window_height_(1000)
{
  init();
}

X11Test::~X11Test()
{
  
}

void X11Test::ClipButtonClickRealizeCallback(GtkWidget *window, 
      GdkEvent  *event, gpointer data)
{
  X11Test * g_this = (X11Test *)data;
  //XClearWindow(g_this->display_, g_this->window_);
 

  // g_this->rectangles.push_back(XRectangle{120, 100, 200, 200});
  // XFillRectangles(g_this->display_, g_this->pixmap_, g_this->gc_, 
  //     g_this->rectangles.data(),  g_this->rectangles.size());


  // gdk_pixbuf_xlib_init(g_this->display_, 0);
  // GdkPixbuf * pixbuf = gdk_pixbuf_xlib_get_from_drawable(NULL,  g_this->pixmap_,
  //     NULL, NULL, 0, 0, 0, 0, g_this->window_width_, g_this->window_height_);
  //     gdk_pixbuf_save(pixbuf, "/code/tmppngs/windowless_paint.bmp",  "bmp",  NULL, "quality", "100", NULL);

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
          100,100,window_width_, window_height_,
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
    gtk_child_window_ = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(gtk_child_window_), 400, 400);
    gtk_window_set_title (GTK_WINDOW (gtk_child_window_), "Hello World");

    GtkWidget *fixed_container = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(gtk_child_window_), fixed_container);


    GtkWidget * clipButton = gtk_button_new_with_label("Button");
    gtk_widget_set_size_request(clipButton, 100, 20);
    gtk_fixed_put((GtkFixed *)fixed_container, clipButton, 200, 200);

    gtk_widget_show(clipButton);
    
  //  gtk_widget_set_app_paintable(window, TRUE);

    g_signal_connect(G_OBJECT(clipButton), "button-press-event",
          G_CALLBACK(ClipButtonClickRealizeCallback), (gpointer)(this));
   
    g_signal_connect(G_OBJECT(gtk_child_window_), "realize",
          G_CALLBACK(ChildWindowRealizeCallback), (gpointer)(this));
   //gtk_widget_realize(button);
    gtk_widget_show_all(gtk_child_window_);
    gtk_main();
  });
  reparent_thread.detach();
}

void X11Test::createPixmap(){
    GdkVisual* sys_visual = gdk_visual_get_system();
    // GdkDrawable* pixmap2 = gdk_pixmap_new(NULL,  pixmap_draw_rect.width(), pixmap_draw_rect.height(),  sys_visual->depth);
    GdkColormap* c = gdk_colormap_new(gdk_visual_get_system(), FALSE);
    // GdkPixbuf *ret = NULL;
    // Drawable drawable = gdk_x11_drawable_get_xid(pixmap2);
    Colormap cmap = gdk_x11_colormap_get_xcolormap(c);
    // Visual* visual = gdk_x11_visual_get_xvisual(sys_visual);

    // GdkDrawable* pixmap2 = gdk_pixmap_new(NULL,  400, 400,  sys_visual->depth);
    // Pixmap pixmap = gdk_x11_drawable_get_xid(pixmap2);

    // Pixmap pixmap  = XCreatePixmap(d, DefaultScreen(d), 400, 400, 1);
    int width  = window_width_/8;
    mask_data_ = new unsigned char[width*window_height_];
    memset(mask_data_, 0xFF, width * window_height_);

    for(int i = 100; i < 200; i++)
      for(int j = 15; j < 25; j+=1)
        mask_data_[i*width + j] = 0;

    // for(int i = 100; i < 200; i++)
    //   for(int j = 15; j < 25; j+=1)
    //     mask_data_[i*width + j] = 0;

    // Visual *visual = XDefaultVisual(display_, 0);
    // visual->bits_per_rgb = 1;

    PRINT_VAL( (int)mask_data_[0]);
    PRINT_VAL( (int)mask_data_[20000-1]);

    
    // XGCValues value;
    // value.background = 0;
    // value.foreground = 1;
    // gc_ = XCreateGC(display_, window_,
    //     GCForeground | GCBackground, &value);

    // rectangles.push_back(XRectangle{120, 100, 200, 200});
    // XRectangle rec[] = {XRectangle{120, 100, 200, 200} };
    // XFillRectangles(display_, pixmap_, gc_, 
    // rec,  1);


    //GdkPixmap *pixmap = gdk_pixmap_new_for_data();




        // pixmap_  = XCreatePixmapFromBitmapData(display_, window_,
        //   (char*)mask_data_, window_width_, window_height_, 1, 0,  1);  
    
    // Pixmap pixmap  = XCreatePixmap(display_, window_, window_width_, window_height_, 1);  

  
    //  cairo_surface_t *surface 
    //              = cairo_xlib_surface_create(display_, pixmap_, NULL, window_width_, window_height_);
    //  cairo_t* dst // = gdk_cairo_create(pixmap2);
    //      =  cairo_create(surface);
    // cairo_save(dst);
    //  cairo_set_source_rgb(dst, 1.0, 1.0, 1.0);
    //  cairo_set_operator (dst, CAIRO_OPERATOR_SOURCE);
    //  cairo_rectangle(dst, 0, 0, window_width_, window_height_);
    //  cairo_clip(dst);
    //  cairo_paint(dst);
    // cairo_restore(dst);

    // cairo_save(dst);
    //  cairo_set_source_rgb(dst, 0.0, 0.0, 0.0);
    //  cairo_set_operator (dst, CAIRO_OPERATOR_SOURCE);
    //  cairo_rectangle(dst, 100, 100, 100, 100);
    //  cairo_clip(dst);
    //  cairo_paint(dst);
    // cairo_restore(dst);


    //  cairo_destroy(dst);

// cairo_image_surface_create_from_png



//     GdkPixmap* pixmap = gdk_pixmap_new(NULL,
//           window_width_,window_height_,1);
//     gdk_draw_rectangle (pixmap,
//             gtk_child_window_->style->white_gc,
//             TRUE,
//             0, 0,
//             window_width_,window_height_);

//     gdk_draw_rectangle (pixmap,
//         gtk_child_window_->style->black_gc,
//         TRUE,
//         100, 100,
//         200, 200);

    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_A1, window_width_);
    cairo_surface_t *surface = cairo_image_surface_create_for_data (mask_data_, CAIRO_FORMAT_A1,
					  window_width_, window_height_,  stride);
    cairo_t* dst // = gdk_cairo_create(pixmap2);
         =  cairo_create(surface);
    cairo_save(dst);
     cairo_set_source_rgb(dst, 0.0, 0.0, 0.0);
     cairo_set_operator (dst, CAIRO_OPERATOR_SOURCE);
     cairo_rectangle(dst, 200, 200, 400, 400);
     cairo_clip(dst);
     cairo_paint(dst);
    cairo_restore(dst);    

    pixmap_  = XCreatePixmapFromBitmapData(display_, window_,
          (char*)mask_data_, window_width_, window_height_, 1, 0,  1);    


    XShapeCombineMask(display_, window_, ShapeBounding, 0, 0, pixmap_, ShapeSet);




    //  gdk_pixbuf_xlib_init(display_, 0);
    //  GdkPixbuf * pixbuf = gdk_pixbuf_xlib_get_from_drawable(NULL,  pixmap,
    //      cmap, NULL, 0, 0, 0, 0, 400, 400);
    //      gdk_pixbuf_save(pixbuf, "/code/tmppngs/windowless_paint.bmp",  "bmp",  NULL, "quality", "100", NULL);
    
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

void X11Test::queryShapeExtension()
{
  int shape_update_event_num = 0;;
  int shape_error = 0;
  auto is_shape_extension_supported = XShapeQueryExtension(display_, &shape_update_event_num, &shape_error);
  PRINT_VAL(is_shape_extension_supported);
}

void testClip()
{
  X11Test x11_test;
  x11_test.queryShapeExtension();
  x11_test.createPixmap();
  X11Test::handleEvents(&x11_test) ;
}