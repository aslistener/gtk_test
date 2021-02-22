#pragma once
#define GDK_PIXBUF_ENABLE_BACKEND


#include "gtk_test/gtk_preserve_window.h"
#include "gtk_test/gtk_plugin_container.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <gdk/gdkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
#include <cairo-xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>

#include <jpeglib.h>
#include <signal.h>

#include <map>
#include <assert.h>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

#define CAIRO_HAS_XLIB_SURFACE 1





#define PRINT_VAL(value) PRINT_VAL_MSG(value,"")
#define PRINT_VAL_MSG(value, message) \
cout << GetCurrentTime() << "  " \
     <<__FUNCTION__ << "  " #value" : " << value <<", " << message << endl;

extern Display *xdisplay_;

GtkWidget* CreatePreservedChild();
GtkWidget* CreateChild();
GdkWindow* CreateRootWindow();

const static std::string GetCurrentTime() {
    std::time_t result = std::time(nullptr);
    static char str_ret[50] = {0};
    memset(str_ret, 0, 50);
    std::strftime(str_ret, 49, "%T", std::localtime(&result));
    return str_ret;
}

void EnumerateWindows (Display *display, Window rootWindow,
      int showErrors, int showStatus, std::vector<unsigned long>* windows);

// print the properties of Xwindow
void GetWindowProperties (Display *display, Window window);

void drawPixmap(GdkDrawable* pixmap, const char* filename, bool draw_circle = false);

static const char *event_names[] = {
   "",
   "",
   "KeyPress",
   "KeyRelease",
   "ButtonPress",
   "ButtonRelease",
   "MotionNotify",
   "EnterNotify",
   "LeaveNotify",
   "FocusIn",
   "FocusOut",
   "KeymapNotify",
   "Expose",
   "GraphicsExpose",
   "NoExpose",
   "VisibilityNotify",
   "CreateNotify",
   "DestroyNotify",
   "UnmapNotify",
   "MapNotify",
   "MapRequest",
   "ReparentNotify",
   "ConfigureNotify",
   "ConfigureRequest",
   "GravityNotify",
   "ResizeRequest",
   "CirculateNotify",
   "CirculateRequest",
   "PropertyNotify",
   "SelectionClear",
   "SelectionRequest",
   "SelectionNotify",
   "ColormapNotify",
   "ClientMessage",
   "MappingNotify"
};

