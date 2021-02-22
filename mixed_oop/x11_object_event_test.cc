#include "x11_object_event_test.h"
#include "x11_objects_methods.h"

namespace testx11 {

  constexpr int kWillAcceptDrop = 1;
  constexpr int kWantFurtherPosEvents = 2;
  const char kXdndActionCopy[] = "XdndActionCopy";
  const char kXdndActionMove[] = "XdndActionMove";
  const char kXdndActionLink[] = "XdndActionLink";

    enum DragOperation {
    DRAG_NONE = 0,
    DRAG_MOVE = 1 << 0,
    DRAG_COPY = 1 << 1,
    DRAG_LINK = 1 << 2
  };

  EventX11Window::EventX11Window() {
    init();
    initX11SubWindow();
    InitDndProperty();
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
        | FocusChangeMask;
    attrs.do_not_propagate_mask = 0; // do not hide any events from child window
    attrs.background_pixel = red.pixel; // background color
    unsigned long attrs_mask
                            = CWEventMask  // enable attrs.event_mask
                            | NoEventMask  // enable attrs.do_not_propagate_mask
                            | CWBackPixel  // enable attrs.background_pixel
    ;
    child_window_=XCreateWindow(display_, window_,
            300, 300, 500, 500,
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
    
    Atom dnd_aware = XInternAtom(display_, "XdndAware", False); 
    XChangeProperty(display_, window_, dnd_aware, XA_ATOM, 32,
                  PropModeReplace,
                  reinterpret_cast<unsigned char*>(&xdnd_version), 1);
    XChangeProperty(display_, child_window_, dnd_aware, XA_ATOM, 32,
              PropModeReplace,
              reinterpret_cast<unsigned char*>(&xdnd_version), 1);
  }

  bool EventX11Window::HandleDndEvent(const XEvent& event) {
    Atom message_type = event.xclient.message_type;
    
    if(message_type == XInternAtom(display_, "XdndEnter", False)) {
      return HandleDndEnterEvent(event.xclient);
    } else if(message_type == XInternAtom(display_, "XdndLeave", False)) {
      return HandleDndLeaveEvent(event.xclient);
    } else if(message_type == XInternAtom(display_, "XdndPosition", False)) {
      return HandleDndPositionEvent(event.xclient);
    } else if(message_type == XInternAtom(display_, "XdndStatus", False)) {
      return HandleDndStatusEvent(event.xclient);
    } else if(message_type == XInternAtom(display_, "XdndFinished", False)) {
      return HandleDndFinishEvent(event.xclient);
    } else if(message_type == XInternAtom(display_, "XdndDrop", False)) {
      return HandleDndDropEvent(event.xclient);
    }
    return false;
  }

  bool EventX11Window::HandleDndEnterEvent(
      const XClientMessageEvent& event) {

    if(event.window == window_) {
      int x, y;
      Window child;
      XWindowAttributes xwa;
      XTranslateCoordinates( display_, window_,
          DefaultRootWindow(display_), 0, 0, &x, &y, &child );
      window_point_.x = x;
      window_point_.y = y;
    } else if(event.window == child_window_) {

    }
    return true;  
  }
  bool EventX11Window::HandleDndLeaveEvent(
      const XClientMessageEvent& event) {
    
    return true;  
  }
  bool EventX11Window::HandleDndPositionEvent(
      const XClientMessageEvent& event) {

  unsigned long source_window = event.data.l[0];
  int x_root_window = event.data.l[2] >> 16;
  int y_root_window = event.data.l[2] & 0xffff;
  cursor_point_.x = x_root_window - window_point_.x;
  cursor_point_.y = y_root_window - window_point_.y;

  DrawDrag(cursor_point_, window_, false);

  ::Time time_stamp = event.data.l[3];
  ::Atom suggested_action = event.data.l[4];

    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.message_type =
        XInternAtom(display_, "XdndStatus", False);
    xev.xclient.format = 32;
    xev.xclient.window = source_window;
    xev.xclient.data.l[0] = window_;
    xev.xclient.data.l[1] = (kWantFurtherPosEvents | kWillAcceptDrop);
    // (drag_operation != 0) ?
    //     (kWantFurtherPosEvents | kWillAcceptDrop) : 0;
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
    return true;  
  }
  bool EventX11Window::HandleDndDropEvent(
      const XClientMessageEvent& event) {

    DrawDrag(cursor_point_, window_, true);
    unsigned long source_window = event.data.l[0];
    
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.message_type =
        XInternAtom(display_, "XdndFinished", False);
    xev.xclient.format = 32;
    xev.xclient.window = source_window;
    xev.xclient.data.l[0] = window_;
    xev.xclient.data.l[1] = 1;
    xev.xclient.data.l[2] = DragOperation::DRAG_COPY;
    XSendEvent(display_, source_window, False, 0, &xev);
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