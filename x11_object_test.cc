#include "x11_test.h"

class X11Test{
public:  
  
  typedef struct{
    int id; 
    XRectangle rec;
  } Rectangle; 
 


  X11Test();
  ~X11Test();
  static void handleEvents(X11Test *p_this);

public:
  void queryShapeExtension();

  void createPixmap();
  void create8BitPixmap();


private:
  void init();
  void initSubWindow();

  void handleExposeEvent(const XEvent &event);
  void handleMapRequestEvent(const XEvent &event);
  void handleMapEvent(const XEvent &event);
  void handleConfigureEvent(const XEvent &event);


  void showRectangle(Rectangle rectangle);
  void hideRectangle(Rectangle rectangle);
  void moveRectangle(int width, int height);

  void calClip(Rectangle rectangle, bool is_clip);


  static int TimeoutFunction(void *) {
    exit(0);
    return 0;
  }
  //  void MoveRectangle(std::vector<Rectangle>& rectangles);

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


  static int global_id;

  Rectangle rectangle_;
  // std::map<int, XRectangle> rectangles_;
  unsigned char* mask_data_ = 0;
  int window_width_;
  int window_height_;
  Pixmap pixmap_ = 0;
};

int X11Test::global_id = 0;

  // Display *display_ = 0;
  // Window window_ = 0;
  // Display *child_display_ = 0;
  // Window child_window_ = 0;
  // GC gc_ = 0;

X11Test::X11Test()
  :window_width_(999),
  window_height_(999)
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

    
  g_this->create8BitPixmap();
  //g_this->create8BitPixmap();
}

void X11Test::ChildWindowRealizeCallback(GtkWidget *window, 
      gpointer data) 
{

  X11Test * g_this = (X11Test *)data;
  GdkDisplay* gdkDisplay = gdk_window_get_display(window->window);
  Display* xDisplay = gdk_x11_display_get_xdisplay(gdkDisplay);

  PRINT_VAL(XConnectionNumber(xDisplay));
  PRINT_VAL(GDK_WINDOW_XID(gtk_widget_get_window(window)));

  g_this->child_window_ = GDK_WINDOW_XID(gtk_widget_get_window(window));
  g_this->child_display_ = xDisplay;

  XReparentWindow(xDisplay, g_this->child_window_, g_this->window_, 400, 400);
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
      | VisibilityChangeMask 
      | LeaveWindowMask
      | ResizeRedirectMask
      | KeyPressMask
      | KeyReleaseMask
      | FocusChangeMask;
  attrs.do_not_propagate_mask = 0; // do not hide any events from child window
  attrs.background_pixel = blue.pixel; // background color
  unsigned long attrs_mask
                          = CWEventMask  // enable attrs.event_mask
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
    // std::this_thread::sleep_for(1s);
    gtk_child_window_ = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(gtk_child_window_), 400, 400);
    gtk_window_set_title (GTK_WINDOW (gtk_child_window_), "Hello World");

    GtkWidget* layout = gtk_layout_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(gtk_child_window_), layout);


    
    
    // GtkWidget *fixed_container = gtk_fixed_new();
    // gtk_container_add(GTK_CONTAINER(gtk_child_window_), fixed_container);

    GtkWidget * clipButton = gtk_button_new_with_label("show");
    gtk_widget_set_size_request(clipButton, 100, 20);
    gtk_layout_put((GtkLayout *)layout, clipButton, 100, 20);


    GtkWidget * hideButton = gtk_button_new_with_label("hide");
    gtk_widget_set_size_request(hideButton, 100, 20);
    gtk_layout_put((GtkLayout *)layout, hideButton, 100, 60);


    GtkWidget * moveButton = gtk_button_new_with_label("move");
    gtk_widget_set_size_request(moveButton, 100, 20);
    gtk_layout_put((GtkLayout *)layout, moveButton, 100, 100);

    GtkWidget * editControl = gtk_text_view_new ();;
    gtk_widget_set_size_request(editControl, 500, 500);
    gtk_layout_put((GtkLayout *)layout, editControl, 300, 300);


    // gtk_widget_set_size_request(clipButton, 1000, 2000);
    // gtk_fixed_put((GtkFixed *)fixed_container, clipButton, 200, 200);

    gtk_widget_show(clipButton);
    gtk_widget_show(hideButton);
    gtk_widget_show(moveButton);
    gtk_widget_show(editControl);
    
   // gtk_widget_set_app_paintable(gtk_child_window_, TRUE);

    g_signal_connect(G_OBJECT(clipButton), "button-press-event",
          G_CALLBACK(ClipButtonClickRealizeCallback), (gpointer)(this));

    g_signal_connect(G_OBJECT(hideButton), "button-press-event",
          G_CALLBACK(ClipButtonClickRealizeCallback), (gpointer)(this));

    g_signal_connect(G_OBJECT(moveButton), "button-press-event",
          G_CALLBACK(ClipButtonClickRealizeCallback), (gpointer)(this));

   
    g_signal_connect(G_OBJECT(gtk_child_window_), "realize",
          G_CALLBACK(ChildWindowRealizeCallback), (gpointer)(this));
   //gtk_widget_realize(button);
    gtk_widget_show_all(gtk_child_window_);

   // gtk_timeout_add(5000, TimeoutFunction, NULL);

    gtk_main();
  });
  reparent_thread.detach();
}

void X11Test::create8BitPixmap(){
  /* convenience */
  int BLACK_PIXEL = BlackPixel(display_, DefaultScreen(display_));
  int WHITE_PIXEL = WhitePixel(display_, DefaultScreen(display_)); 

  XGCValues shape_xgcv;
  shape_xgcv.background = BLACK_PIXEL;
  shape_xgcv.foreground = WHITE_PIXEL;

  int width = window_width_/4;
  int height = window_height_/4;
  Pixmap pmap = XCreatePixmap(child_display_, child_window_, width, height, 1);
  GC shape_gc = XCreateGC(child_display_, pmap, GCForeground | GCBackground, &shape_xgcv);
  
  XSetForeground(child_display_, shape_gc, WHITE_PIXEL);
  XFillRectangle(child_display_, pmap, shape_gc, 0, 0, width, height);  
  
  XSetForeground(child_display_, shape_gc, BLACK_PIXEL);
  XFillRectangle(child_display_, pmap, shape_gc, 100, 100, 100, 100);  

  XShapeCombineMask (child_display_, child_window_, ShapeBounding,
          0, 0, pmap, ShapeSet);

}

void X11Test::createPixmap(){
  // return create8BitPixmap();


    static unsigned char b = 0;
    b = (b==0? 0XFF: 0);

    int width  = 400; //window_width_/8+1;

    mask_data_ = new unsigned char[width*window_height_];
    memset(mask_data_, 0xFF, width * window_height_);

    for(int i = 100; i < 200; i++)
      for(int j = 15; j < 25; j+=1)
        mask_data_[i*width + j] = b;
        
    // pixmap_  = XCreateBitmapFromData(display_, window_,
    //       (char*)mask_data_ , width*8, window_height_);     


    // XShapeCombineMask(display_, window_, ShapeBounding, 0, 0, pixmap_, ShapeSet);
    

    for(int i = 100; i < 200; i++)
    for(int j = 30; j < 40; j+=1)
      mask_data_[i*width + j] = b;

    if(pixmap_){
      XFreePixmap(display_, pixmap_);
      pixmap_ = 0;
    }

    pixmap_  = XCreateBitmapFromData(display_, window_,
          (char*)mask_data_ , width*8, window_height_);     

    XShapeCombineMask(display_, window_, ShapeBounding, 0, 0, pixmap_, ShapeSet);    
}


void X11Test::handleEvents(X11Test *p_this)
{
 // XGrabKeyboard(p_this->display_, p_this->child_window_, true, GrabModeAsync, GrabModeSync, CurrentTime);
  // XUngrabPointer(p_this->display_, CurrentTime);
 // XGrabButton(p_this->display_, p_this->child_window_, )
 // XSelectInput(p_this->display_, p_this->child_window_, KeyPressMask|KeyReleaseMask); 




  Atom wm_delete = XInternAtom(p_this->display_, "WM_DELETE_WINDOW", True);
  // Subscribe WM_DELETE_WINDOW message
  XSetWMProtocols(p_this->display_, p_this->window_, &wm_delete, 1);
  // XGrabServer(p_this->display_);
  
  bool is_running = true;
  while(is_running) 
  {
    XEvent event;
    XGenericEventCookie *cookie = &event.xcookie;
    XClientMessageEvent &client_ev = event.xclient;
    XNextEvent(p_this->display_, &event);
    PRINT_VAL(event.type);
   // PRINT_VAL(event_names[event.type]);
   // PRINT_VAL(event.xmap.window);
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
      //case EnterNotify:

      case LeaveNotify:{
        PRINT_VAL_MSG(LeaveNotify, "leave event");
        if(event.xkey.window == p_this->child_window_) {
          PRINT_VAL_MSG(LeaveNotify, " leave child window");
          XSetInputFocus(p_this->child_display_, p_this->child_window_, RevertToPointerRoot, CurrentTime);

        }
      }

      case KeyPress:
      case KeyRelease:
      {
        if(event.xkey.window == p_this->window_) {
          PRINT_VAL_MSG(event.type, "send key to child window");
          XKeyEvent key_event = event.xkey;
          long mask = (event.type == KeyPress)? KeyPressMask: KeyReleaseMask;
          event.xkey.window = p_this->child_window_;
          XSendEvent(p_this->display_, p_this->child_window_, TRUE, mask, (XEvent *)&event.xkey); 
        } else if(event.xkey.window == p_this->child_window_) {
          PRINT_VAL_MSG(event.type, "child window received key");
        }

      }      
      case Expose:
        p_this->handleExposeEvent(event);
      break;
      case MapNotify:
        p_this->handleMapEvent(event);
      break;
      case ButtonPress:
       cout << "button pressed " << endl;
        //p_this->handleMapEvent(event);
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
  // XDrawRectangle(display_, child_window_, gc_, 100, 100, 200, 200);

  if(child_window_)
    XDrawRectangle(display_, child_window_, gc_, 100, 100, 200, 200);
  if(event.xexpose.window == child_window_) 
  {
      XDrawRectangle(child_display_, child_window_, gc_, 100, 100, 200, 200);
  }

 // XDrawRectangle(display_, window_, gc_, 0, 0, 200, 200);


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

// void X11Test::MoveRectangle(std::vector<Rectangle>& rectangles)
// {


// }



void testClip()
{
  X11Test x11_test;
  x11_test.queryShapeExtension();
 // x11_test.createPixmap();
  X11Test::handleEvents(&x11_test) ;
}