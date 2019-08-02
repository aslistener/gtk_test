#pragma once

#include <X11/X.h>
#include <X11/Xlib.h>
#include <iostream>

using namespace std;

#include "gtk_preserve_window.h"
#include "gtk_plugin_container.h"



extern Display *xdisplay_;

GtkWidget* CreatePreservedChild();
GtkWidget* CreateChild();
GtkWidget* CreateRootWindow();


