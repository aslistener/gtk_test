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

  // custom event                                    
  static void ClipButtonClickRealizeCallback(GtkWidget* window,
                                             GdkEvent* event,
                                             gpointer data);
  void HandleButtonClickRealizeCallback(GtkWidget* window,
                                        GdkEvent* event,
                                        gpointer data);

 private:
  void initSubGtkWindow();
  void initX11SubWindow();

 private:
};
}  // namespace testx11