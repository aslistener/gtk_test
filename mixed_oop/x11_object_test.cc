#include "x11_object_test.h"
#include "x11_objects_methods.h"

namespace testx11 {
  namespace internal {

    int CustomX11WindowBase::global_id = 0;

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
  }  // namespace internal

  void TestObject(){
    internal::CustomX11WindowBase x11_test;
    //x11_test.queryShapeExtension();
    internal::CustomX11WindowBase::handleEvents(&x11_test) ;
  }
}  // namespace testx11

