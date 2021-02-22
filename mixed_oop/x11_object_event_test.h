#pragma once
#include "x11_object_test.h"

namespace testx11 {
class EventX11Window : public internal::CustomX11WindowBase {
 public:
  EventX11Window();
  ~EventX11Window() override;

 protected:
  void HandleExposeEvent(const XEvent& event) override;
  void HandleMapRequestEvent(const XEvent& event) override;
  void HandleConfigureEvent(const XEvent& event) override;
  bool HandleEventInternal(const XEvent& event) override;
  void HandleChildWindowRealizeCallback(GtkWidget* window,
                                        gpointer data) override;
  void HandleMainWindowRealizeCallback() override;

  // custom event                                    
  static void ClipButtonClickRealizeCallback(GtkWidget* window,
                                             GdkEvent* event,
                                             gpointer data);
  void HandleButtonClickRealizeCallback(GtkWidget* window,
                                        GdkEvent* event,
                                        gpointer data);

 private:
  void InitDndProperty();
//  void initSubGtkWindow();
  void initX11SubWindow();

  bool HandleDndEvent(const XEvent& event);
  bool HandleDndEnterEvent(const XClientMessageEvent& event);
  bool HandleDndLeaveEvent(const XClientMessageEvent& event);
  bool HandleDndPositionEvent(const XClientMessageEvent& event);
  bool HandleDndStatusEvent(const XClientMessageEvent& event);
  bool HandleDndFinishEvent(const XClientMessageEvent& event);
  bool HandleDndDropEvent(const XClientMessageEvent& event);

  void DrawDrag(const XPoint& p, Window w, bool is_drop);

private:
  XPoint cursor_point_;
  XPoint window_point_;
  XPoint child_window_point_;
};
}  // namespace testx11