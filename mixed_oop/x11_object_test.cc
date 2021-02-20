#include "x11_object_test.h"

namespace testx11 {
  namespace internal {

    int CustomX11WindowBase::global_id = 0;

    void CustomX11WindowBase
        ::ClipButtonClickRealizeCallback(GtkWidget *window, 
        GdkEvent  *event, gpointer data) {
      CustomX11WindowBase * g_this =
          (CustomX11WindowBase *)data;
      if(g_this)
        g_this->HandleButtonClickRealizeCallback(
            window, event, data);
    }

    void CustomX11WindowBase
        ::ChildWindowRealizeCallback(GtkWidget *window, 
          gpointer data) {
      CustomX11WindowBase * g_this =
          (CustomX11WindowBase *)data;
      if(g_this) {
        g_this->HandleChildWindowRealizeCallback(window, data);

        GdkDisplay* gdkDisplay = gdk_window_get_display(window->window);
        Display* xDisplay = gdk_x11_display_get_xdisplay(gdkDisplay);
        // PRINT_VAL(XConnectionNumber(xDisplay));
        // PRINT_VAL(GDK_WINDOW_XID(gtk_widget_get_window(window)));
        g_this->child_window_ = GDK_WINDOW_XID(gtk_widget_get_window(window));
        g_this->child_display_ = 0;
        XReparentWindow(g_this->display_, g_this->child_window_, g_this->window_, 400, 400);
      }
    }
  
    CustomX11WindowBase::CustomX11WindowBase()
      :window_width_(999),
      window_height_(999) {
    //  init();
    }

    CustomX11WindowBase::~CustomX11WindowBase() {
      
    }

    void CustomX11WindowBase::init()
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

      Atom del_window = XInternAtom(display_, "WM_DELETE_WINDOW", 0);
      XSetWMProtocols(display_ , window_, &del_window, 1);
      
      XMapWindow(display_, window_);
    }

    void CustomX11WindowBase::initSubGtkWindow()
    {
      std::thread reparent_thread([&, this](){
        std::this_thread::sleep_for(1s);
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
        // gtk_widget_realize(button);
        gtk_widget_show_all(gtk_child_window_);

        // gtk_timeout_add(5000, TimeoutFunction, NULL);
        gtk_main ();

      });
  //    XSync(display_, 0);
      reparent_thread.detach();
    }

    void CustomX11WindowBase::initX11SubWindow() {

    }

    void CustomX11WindowBase::handleEvents(
        CustomX11WindowBase *p_this) {
    // XGrabKeyboard(p_this->display_, p_this->child_window_, true, GrabModeAsync, GrabModeSync, CurrentTime);
    // XUngrabPointer(p_this->display_, CurrentTime);
    // XGrabButton(p_this->display_, p_this->child_window_, )
    // XSelectInput(p_this->display_, p_this->child_window_, KeyPressMask|KeyReleaseMask); 

      Atom wm_delete = XInternAtom(p_this->display_, "WM_DELETE_WINDOW", True);
      // Subscribe WM_DELETE_WINDOW message
      XSetWMProtocols(p_this->display_, p_this->window_, &wm_delete, 1);
      // XGrabServer(p_this->display_);
      
      printf("display id: %xd, child display id: %xd\n",
          p_this->display_, p_this->child_display_);

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
        if(p_this->HandleEventInternal(event)) {
          continue;
        }
        switch(event.type)
        {
          case MapRequest:
            p_this->HandleMapRequestEvent(event);
          break;
          case ConfigureNotify:
          {
            p_this->HandleConfigureEvent(event);
            break;
          }     
          case Expose:
            p_this->HandleExposeEvent(event);
          break;
          case MapNotify:
            p_this->HandleMapEvent(event);
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

    void CustomX11WindowBase::
        HandleMapRequestEvent(const XEvent &event) {
    }

    void CustomX11WindowBase
        ::HandleMapEvent(const XEvent &event) {  
    }

    void CustomX11WindowBase::
        HandleExposeEvent(const XEvent &event) {
    }

    bool CustomX11WindowBase::
        HandleEventInternal(const XEvent &event) {
      return false;
    }

    void CustomX11WindowBase::queryShapeExtension() {
      int shape_update_event_num = 0;;
      int shape_error = 0;
      auto is_shape_extension_supported = XShapeQueryExtension(
          display_, &shape_update_event_num, &shape_error);
      PRINT_VAL(is_shape_extension_supported);
    }

    void CustomX11WindowBase::
        HandleConfigureEvent(const XEvent &event) {}

    void CustomX11WindowBase::
        HandleChildWindowRealizeCallback(
        GtkWidget *window,
        gpointer data) {}
    void CustomX11WindowBase::
        HandleMainWindowRealizeCallback(
        GtkWidget *window,
        gpointer data){}
    void CustomX11WindowBase::
        HandleButtonClickRealizeCallback(
        GtkWidget *window,
        GdkEvent  *event,
        gpointer data){}
  }  // namespace internal

  void TestObject(){
    internal::CustomX11WindowBase x11_test;
    //x11_test.queryShapeExtension();
    internal::CustomX11WindowBase::handleEvents(&x11_test) ;
  }
}  // namespace testx11

