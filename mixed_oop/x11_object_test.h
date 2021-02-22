#pragma once
#include "functions.h"

namespace testx11 {
  namespace internal {
    class CustomX11WindowBase {
    public:  
      CustomX11WindowBase();
      virtual ~CustomX11WindowBase();
      static void handleEvents(CustomX11WindowBase *p_this);

    protected:
      void init();
      void queryShapeExtension();
      //void initSubGtkWindow();
      //void initX11SubWindow();

      // x11 event callbacks
      virtual void HandleExposeEvent(const XEvent &event);
      virtual void HandleMapRequestEvent(const XEvent &event);
      virtual void HandleMapEvent(const XEvent &event);
      virtual void HandleConfigureEvent(const XEvent &event);
      virtual bool HandleEventInternal(const XEvent &event);

      // gtk event callbacks
      virtual void HandleChildWindowRealizeCallback(
          GtkWidget *window,
          gpointer data);
      virtual void HandleMainWindowRealizeCallback();

      static void WindowRealizeCallback(GtkWidget *window, 
          gpointer data);
      static void ChildWindowRealizeCallback(GtkWidget *window, 
          gpointer data);

    protected:
      Display *display_ = 0;
      Window window_ = 0;
      Display *child_display_ = 0;
      Window child_window_ = 0;
      GtkWidget *gtk_child_window_ = 0;
      GC gc_ = 0;

      static int global_id;

      int window_width_;
      int window_height_;

    };
  }
}

