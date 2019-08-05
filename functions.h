#pragma once

#include "gtk_preserve_window.h"
#include "gtk_plugin_container.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <gdk/gdkx.h>
#include <assert.h>
#include <chrono>
#include <thread>

#include <iostream>

using namespace std;

extern Display *xdisplay_;

GtkWidget* CreatePreservedChild();
GtkWidget* CreateChild();
GdkWindow* CreateRootWindow();

int TestOpenDisplay();
int test_main(int argc, char** argv);

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

