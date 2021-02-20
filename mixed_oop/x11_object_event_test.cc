#include "x11_object_event_test.h"
#include "x11_objects_methods.h"

namespace testx11 {
  EventX11Window::EventX11Window() {
    init();
    initSubGtkWindow();
  }

  EventX11Window::~EventX11Window(){

  }

  void EventX11Window::initSubGtkWindow()
    {
      std::thread reparent_thread([&, this](){
        std::this_thread::sleep_for(1s);
        gtk_init (NULL, NULL);
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

  void EventX11Window::initX11SubWindow() {
  }

  void EventX11Window::HandleExposeEvent(const XEvent &event) {

  }
  void EventX11Window::HandleMapRequestEvent(const XEvent &event) {

  }
  void EventX11Window::HandleConfigureEvent(const XEvent &event) {
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

  void EventX11Window
      ::ClipButtonClickRealizeCallback(GtkWidget *window, 
      GdkEvent  *event, gpointer data) {
    EventX11Window * g_this =
        (EventX11Window *)data;
    if(g_this)
      g_this->HandleButtonClickRealizeCallback(
          window, event, data);
  }

  void testEvent() {
     EventX11Window x11_test;
     x11_test.handleEvents(&x11_test);
  }
}


