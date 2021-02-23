#include "x11_object_event_test.h"
#include "x11_objects_methods.h"

namespace testx11 {

  constexpr int kWillAcceptDrop = 1;
  constexpr int kWantFurtherPosEvents = 2;
  const char kXdndActionCopy[] = "XdndActionCopy";
  const char kXdndActionMove[] = "XdndActionMove";
  const char kXdndActionLink[] = "XdndActionLink";

  Atom dnd_aware_atom;
  Atom dnd_enter_atom;
  Atom dnd_position_atom;
  Atom dnd_status_atom;
  Atom dnd_finish_atom;
  Atom dnd_drop_atom;
  Atom dnd_leave_atom;
  Atom dnd_proxy_atom;


    enum DragOperation {
    DRAG_NONE = 0,
    DRAG_MOVE = 1 << 0,
    DRAG_COPY = 1 << 1,
    DRAG_LINK = 1 << 2
  };

  inline bool IsWindowContainsPoint(XPoint p,
      int x, int y, int w = 300, int h = 300) {
    return p.x >= x && p.y >= y &&
           p.x <= x + w && p.y <= y + h;
  }



  EventX11Window::EventX11Window() {
    init();
    initX11SubWindow();
    InitDndProperty();
    printf("window is %lx, child is %lx\n", window_, child_window_);

  }

  EventX11Window::~EventX11Window(){

  }

  void EventX11Window::initX11SubWindow() {
    int screen=DefaultScreen(display_);
    Colormap colormap = DefaultColormap(display_, screen);
    XColor red;
    XAllocNamedColor(display_, colormap, "red", &red, &red);
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
        | FocusChangeMask
        | PointerMotionMask
        | ButtonMotionMask;
    attrs.do_not_propagate_mask = 0; // do not hide any events from child window
    attrs.background_pixel = red.pixel; // background color
    unsigned long attrs_mask
                            = CWEventMask  // enable attrs.event_mask
                            | NoEventMask  // enable attrs.do_not_propagate_mask
                            | CWBackPixel  // enable attrs.background_pixel
    ;
    child_window_=XCreateWindow(display_, window_,
            150, 150, 200, 200,
            1,CopyFromParent,InputOutput,CopyFromParent,
            attrs_mask,&attrs);
    XMapWindow(display_, child_window_);
  }

  void EventX11Window::HandleExposeEvent(const XEvent &event) {

  }
  void EventX11Window::HandleMapRequestEvent(const XEvent &event) {

  }
  void EventX11Window::HandleConfigureEvent(const XEvent &event) {
    HandleMainWindowRealizeCallback();
    // if (child_window_) {
    //   // Get container window attributes
    //   XWindowAttributes attrs;
    //   XGetWindowAttributes(display_, window_, &attrs);
    //   // Move and resize child
    //   XMoveResizeWindow(child_display_,
    //                     child_window_,
    //                     100, 100, attrs.width - 200, attrs.height - 200);
    //   gc_ = XCreateGC(child_display_, child_window_, 0, 0);
    //   XSetBackground (display_, gc_, BlackPixel (display_, DefaultScreen(display_)));
    // }
  }

  bool EventX11Window::HandleEventInternal(const XEvent &event) {
    if(event.type == ClientMessage) {
      if(event.xclient.message_type ==
          XInternAtom(display_, "WM_PROTOCOLS", False))
        return false;
      else
        return HandleDndEvent(event);
    } else if(event.type == MotionNotify &&
        event.xbutton.window == child_window_) {
      HandleChildMouseMove(event);
    }
    return false;
  }
  void EventX11Window::HandleButtonClickRealizeCallback(
      GtkWidget *window,
      GdkEvent  *event,
      gpointer data){
  }
  void EventX11Window::HandleChildWindowRealizeCallback(
        GtkWidget *window,
        gpointer data){

  }

  void EventX11Window::HandleMainWindowRealizeCallback() {
  }

  void EventX11Window
      ::ClipButtonClickRealizeCallback(GtkWidget *window, 
      GdkEvent  *event, gpointer data) {
    EventX11Window * g_this =
        (EventX11Window *)data;
    if(g_this)
      g_this->HandleButtonClickRealizeCallback(
          window, event, data);
  }

  void EventX11Window::InitDndProperty(){
    unsigned long xdnd_version = 5;
    
    dnd_aware_atom = XInternAtom(display_, "XdndAware", False);
    dnd_enter_atom = XInternAtom(display_, "XdndEnter", False);
    dnd_position_atom = XInternAtom(display_, "XdndPosition", False);
    dnd_status_atom = XInternAtom(display_, "XdndStatus", False);
    dnd_finish_atom = XInternAtom(display_, "XdndFinished", False);
    dnd_drop_atom = XInternAtom(display_, "XdndDrop", False);
    dnd_leave_atom = XInternAtom(display_, "XdndLeave", False);
    dnd_proxy_atom = XInternAtom(display_, "XdndProxy", False);
     
    XChangeProperty(display_, window_, dnd_aware_atom, XA_ATOM, 32,
                  PropModeReplace,
                  reinterpret_cast<unsigned char*>(&xdnd_version), 1);
    XChangeProperty(display_, child_window_, dnd_aware_atom, XA_ATOM, 32,
              PropModeReplace,
              reinterpret_cast<unsigned char*>(&xdnd_version), 1);
    
    XChangeProperty(display_, child_window_,
        dnd_proxy_atom, XA_WINDOW, 32, PropModeReplace, (unsigned char*)&child_window_, 1);

    XChangeProperty(display_, window_, dnd_proxy_atom, XA_WINDOW, 32,
          PropModeReplace,
          reinterpret_cast<unsigned char*>(&child_window_), 1);
  }

  bool EventX11Window::
      HandleChildMouseMove(const XEvent &xev) {
    // if(is_in_dnd_drop_process_) {
    //   XEvent xev;
    //   xev.xclient.type = ClientMessage;
    //   xev.xclient.message_type = 
    //       dnd_position_atom;
    //   xev.xclient.format = 32;
    //   xev.xclient.window = source_window_;
    //   xev.xclient.data.l[0] = child_window_;
    //   xev.xclient.data.l[1] = 0;
    //   xev.xclient.data.l[2] =
    //       (xev.xbutton.x_root << 16) | xev.xbutton.y_root;
    //   xev.xclient.data.l[3] = 0;
    //   xev.xclient.data.l[4] = DragOperation::DRAG_COPY;
    //   XSendEvent(display_, source_window_, False, 0, &xev);  
    // }
    return false;
  }

  bool EventX11Window::HandleDndEvent(const XEvent& event) {
    Atom message_type = event.xclient.message_type;
    
    if(message_type == dnd_enter_atom) {
      printf("event:  %s, window: %lx\n", "XdndEnter", event.xclient.window);
      return HandleDndEnterEvent(event.xclient);
    } else if(message_type == dnd_leave_atom) {
      printf("event:  %s, window: %lx\n", "XdndLeave", event.xclient.window);
      return HandleDndLeaveEvent(event.xclient);
    } else if(message_type == dnd_position_atom) {
      printf("event:  %s, window: %lx\n", "XdndPosition", event.xclient.window);
      return HandleDndPositionEvent(event.xclient);
    } else if(message_type == dnd_status_atom) {
      printf("event:  %s, window: %lx\n", "XdndStatus", event.xclient.window);
      return HandleDndStatusEvent(event.xclient);
    } else if(message_type == dnd_finish_atom) {
      printf("event:  %s, window: %lx\n", "XdndFinished", event.xclient.window);
      return HandleDndFinishEvent(event.xclient);
    } else if(message_type == dnd_drop_atom) {
      printf("event:  %s, window: %lx\n", "XdndDrop", event.xclient.window);
      return HandleDndDropEvent(event.xclient);
    }
    return false;
  }

  bool EventX11Window::HandleDndEnterEvent(
      const XClientMessageEvent& event) {
    source_window_ = event.data.l[0];
    is_in_dnd_drop_process_ = true;
    int x, y;
    Window child;
    XWindowAttributes xwa;

    XTranslateCoordinates( display_, window_,
        DefaultRootWindow(display_), 0, 0, &x, &y, &child );
    window_point_.x = x;
    window_point_.y = y;

    XTranslateCoordinates( display_, child_window_,
        DefaultRootWindow(display_), 0, 0, &x, &y, &child );
    child_window_point_.x = x;
    child_window_point_.y = y;

    if(event.window == window_) {
        is_in_child_area_ = false;
        
    } else if(event.window == child_window_) {
      printf("child window entered\n");
      XEvent xev;
      xev.xclient.type = ClientMessage;
      xev.xclient.message_type =
          dnd_status_atom;
      xev.xclient.format = 32;
      xev.xclient.window = event.data.l[0];
      xev.xclient.data.l[0] = child_window_;
      xev.xclient.data.l[1] = 0;
      xev.xclient.data.l[2] = 0;
      xev.xclient.data.l[3] = 0;
      xev.xclient.data.l[4] = 0;
      XSendEvent(display_, event.data.l[0], False, 0, &xev);
    }
    return true;  
  }
  bool EventX11Window::HandleDndLeaveEvent(
      const XClientMessageEvent& event) {
    is_in_dnd_drop_process_ = false;
    return true;  
  }
  bool EventX11Window::HandleDndPositionEvent(
      const XClientMessageEvent& event) {

    unsigned long source_window = event.data.l[0];
    int screen_x  = event.data.l[2] >> 16;
    int screen_y = event.data.l[2] & 0xffff;
    ::Window dest = event.window;

    bool in_dnd_position = true;
    if(event.window == window_) {
      cursor_point_.x = screen_x - window_point_.x;
      cursor_point_.y = screen_y - window_point_.y;
      if (IsWindowContainsPoint(XPoint{screen_x, screen_y},
          child_window_point_.x, child_window_point_.y)) {
        if(!is_in_child_area_) {
          // enter event on child window
          in_dnd_position = false;
          is_in_child_area_ = true;
        } else {
          // position event on child window
          printf("DndPositionEvent is_in_child_area, \n");
          dest = child_window_;
        }
      }
  } else if(event.window == child_window_){
    is_in_child_area_ = true;
    cursor_point_.x = screen_x - window_point_.x - 150;
    cursor_point_.y = screen_y - window_point_.y - 150;
    printf("DndPositionEvent on child, %u, %u\n",
        cursor_point_.x, cursor_point_.y);
  }

  DrawDrag(cursor_point_, dest, false);

    if(!in_dnd_position) {
      // enter event
      XEvent xev;
      xev.type = ClientMessage;
      xev.xclient.message_type =
          dnd_enter_atom;
      xev.xclient.format = 32;
      xev.xclient.window = child_window_;
      xev.xclient.data.l[0] = source_window;
      XSendEvent(display_, child_window_, False, 0, &xev);
    } else if(is_in_child_area_ && event.window != child_window_) {
      // position event
      XEvent xev;
      xev.type = ClientMessage;
      xev.xclient = event;
      xev.xclient.window = child_window_;
      XSendEvent(display_, child_window_, False, 0, &xev);
    }

    ::Time time_stamp = event.data.l[3];
    ::Atom suggested_action = event.data.l[4];
  
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.message_type = dnd_status_atom;
    xev.xclient.format = 32;
    xev.xclient.window = source_window;
  
    xev.xclient.data.l[0] = event.window;
  
    // if(is_in_child_area_)
    //   xev.xclient.data.l[0] = child_window_;
    // else
    //   xev.xclient.data.l[0] = window_;
    xev.xclient.data.l[1] = //(kWantFurtherPosEvents | kWillAcceptDrop);
        in_dnd_position ?
        (kWantFurtherPosEvents | kWillAcceptDrop) : 0;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 0;
    xev.xclient.data.l[4] = suggested_action;
    XSendEvent(display_, source_window, False, 0, &xev);

    return true;  
  }


  bool EventX11Window::HandleDndStatusEvent(
      const XClientMessageEvent& event) {
    return true;  
  }
  bool EventX11Window::HandleDndFinishEvent(
      const XClientMessageEvent& event) {
    is_in_dnd_drop_process_ = false;
    is_in_child_area_ = false;
    return true;  
  }
  bool EventX11Window::HandleDndDropEvent(
      const XClientMessageEvent& event) {
    is_in_dnd_drop_process_ = false;
    ::Window dest = event.window;
    if(is_in_child_area_ && event.window != child_window_) {
      dest = child_window_;
      in_dnd_child_process = false;
      XEvent xev;
      xev.type = ClientMessage;
      xev.xclient = event;
      xev.xclient.window = child_window_;
      XSendEvent(display_, child_window_, False, 0, &xev);
      return true;
    }

    DrawDrag(cursor_point_, dest, true);
    unsigned long source_window = event.data.l[0];
    
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.message_type =
        dnd_finish_atom;
    xev.xclient.format = 32;
    xev.xclient.window = source_window;
    xev.xclient.data.l[0] = event.window;
    xev.xclient.data.l[1] = 1;
    xev.xclient.data.l[2] = DragOperation::DRAG_COPY;
    XSendEvent(display_, source_window, False, 0, &xev);
    
    is_in_child_area_ = false;

    return true;  
  }

  void EventX11Window::DrawDrag(
      const XPoint& p, Window w, bool is_drop) {
    static GC gc = XCreateGC(display_, w, 0, 0);

    if(is_drop) {
      XSetBackground (display_, gc, BlackPixel (display_, DefaultScreen(display_))); 
      XDrawRectangle(display_, w, gc, cursor_point_.x, cursor_point_.y, 100, 100);
    } else {
      XSetBackground (display_, gc, BlackPixel (display_, DefaultScreen(display_))); 
      XDrawRectangle(display_, w, gc, cursor_point_.x, cursor_point_.y, 10, 10);
    }

    //XFreeGC()
  }


  void testEvent() {
     EventX11Window x11_test;
     x11_test.handleEvents(&x11_test);
  }
}

// void EventX11Window::initSubGtkWindow()
//     {
//       std::thread reparent_thread([&, this](){
//         //std::this_thread::sleep_for(1s);
//         gtk_child_window_ = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//         gtk_window_set_default_size(GTK_WINDOW(gtk_child_window_), 400, 400);
//         gtk_window_set_title (GTK_WINDOW (gtk_child_window_), "Hello World");

//         GtkWidget* layout = gtk_layout_new(NULL, NULL);
//         gtk_container_add(GTK_CONTAINER(gtk_child_window_), layout);
      
//         // GtkWidget *fixed_container = gtk_fixed_new();
//         // gtk_container_add(GTK_CONTAINER(gtk_child_window_), fixed_container);

//         GtkWidget * clipButton = gtk_button_new_with_label("show");
//         gtk_widget_set_size_request(clipButton, 100, 20);
//         gtk_layout_put((GtkLayout *)layout, clipButton, 100, 20);


//         GtkWidget * hideButton = gtk_button_new_with_label("hide");
//         gtk_widget_set_size_request(hideButton, 100, 20);
//         gtk_layout_put((GtkLayout *)layout, hideButton, 100, 60);


//         GtkWidget * moveButton = gtk_button_new_with_label("move");
//         gtk_widget_set_size_request(moveButton, 100, 20);
//         gtk_layout_put((GtkLayout *)layout, moveButton, 100, 100);

//         GtkWidget * editControl = gtk_text_view_new ();;
//         gtk_widget_set_size_request(editControl, 500, 500);
//         gtk_layout_put((GtkLayout *)layout, editControl, 300, 300);


//         // gtk_widget_set_size_request(clipButton, 1000, 2000);
//         // gtk_fixed_put((GtkFixed *)fixed_container, clipButton, 200, 200);

//         gtk_widget_show(clipButton);
//         gtk_widget_show(hideButton);
//         gtk_widget_show(moveButton);
//         gtk_widget_show(editControl);
        
//        gtk_widget_set_app_paintable(gtk_child_window_, TRUE);

//         g_signal_connect(G_OBJECT(clipButton), "button-press-event",
//               G_CALLBACK(ClipButtonClickRealizeCallback), (gpointer)(this));

//         g_signal_connect(G_OBJECT(hideButton), "button-press-event",
//               G_CALLBACK(ClipButtonClickRealizeCallback), (gpointer)(this));

//         g_signal_connect(G_OBJECT(moveButton), "button-press-event",
//               G_CALLBACK(ClipButtonClickRealizeCallback), (gpointer)(this));

      
//         g_signal_connect(G_OBJECT(gtk_child_window_), "realize",
//               G_CALLBACK(ChildWindowRealizeCallback), (gpointer)(this));
//         // gtk_widget_realize(button);
//         gtk_widget_show_all(gtk_child_window_);

//         // gtk_timeout_add(5000, TimeoutFunction, NULL);
//         gtk_main ();

//       });
//   //    XSync(display_, 0);
//       reparent_thread.detach();
//     }