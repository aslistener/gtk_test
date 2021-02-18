#include "x11_object_test.h"
#include "x11_test.h"

namespace testx11 {

  class EventX11Window:
      public internal::CustomX11WindowBase {
  public:
    EventX11Window();
    ~EventX11Window() override;

  protected:
    void HandleExposeEvent(const XEvent &event) override;
    void HandleMapRequestEvent(const XEvent &event) override;
    void HandleConfigureEvent(const XEvent &event) override;
    bool HandleEventInternal(const XEvent &event) override;

    void HandleButtonClickRealizeCallback(
        GtkWidget *window,
        GdkEvent  *event,
        gpointer data) override;
    void HandleChildWindowRealizeCallback(
          GtkWidget *window,
          gpointer data) override;

  private:

  private:
  };

  EventX11Window::EventX11Window() {
    init();
    initSubGtkWindow();

  }

  EventX11Window::~EventX11Window(){

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

  void testEvent() {
    //  EventX11Window x11_test;
    //  x11_test.handleEvents(&x11_test);
    // gtk_main ();
  }
}


