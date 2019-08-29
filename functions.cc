#include "functions.h"

#include "xlib_test/xshm.h"

Display *xdisplay_ = NULL;

GtkWidget* CreatePreservedChild()
{
  GtkWidget* widget = gtk_preserve_window_new();
    // GtkWidget* widget = gtk_frame_new("hello");

  const GdkColor kBGColor = 
    { 0, 0xff, 0xff , 0x00  };

    gtk_widget_set_name(widget, "chrome-render-widget-host-view");
    // We manually double-buffer in Paint() because Paint() may or may not be
    // called in repsonse to an "expose-event" signal.
    gtk_widget_set_double_buffered(widget, FALSE);
    gtk_widget_set_redraw_on_allocate(widget, FALSE);
    gtk_widget_modify_bg(widget, GTK_STATE_NORMAL, &kBGColor);

  //  gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, );

    // Allow the browser window to be resized freely.
    gtk_widget_set_size_request(widget, 50,50);

    gtk_widget_add_events(widget, GDK_EXPOSURE_MASK |
                                  GDK_STRUCTURE_MASK |
                                  GDK_POINTER_MOTION_MASK |
                                  GDK_BUTTON_PRESS_MASK |
                                  GDK_BUTTON_RELEASE_MASK |
                                  GDK_KEY_PRESS_MASK |
                                  GDK_KEY_RELEASE_MASK |
                                  GDK_FOCUS_CHANGE_MASK |
                                  GDK_ENTER_NOTIFY_MASK |
                                  GDK_LEAVE_NOTIFY_MASK | 
                                  GDK_VISIBILITY_NOTIFY_MASK);
    gtk_widget_set_can_focus(widget, TRUE);

    // g_signal_connect(widget, "expose-event",
    //                  G_CALLBACK(OnExposeEvent), host_view);
    // g_signal_connect(widget, "realize",
    //                  G_CALLBACK(OnRealize), host_view);
    // g_signal_connect(widget, "configure-event",
    //                  G_CALLBACK(OnConfigureEvent), host_view);
    // g_signal_connect(widget, "size-allocate",
    //                  G_CALLBACK(OnSizeAllocate), host_view);
/*
    g_signal_connect(widget, "key-press-event",
                     G_CALLBACK(OnKeyPressReleaseEvent), host_view);
    g_signal_connect(widget, "key-release-event",
                     G_CALLBACK(OnKeyPressReleaseEvent), host_view);
    g_signal_connect(widget, "focus-in-event",
                     G_CALLBACK(OnFocusIn), host_view);
    g_signal_connect(widget, "focus-out-event",
                     G_CALLBACK(OnFocusOut), host_view);
    g_signal_connect(widget, "grab-notify",
                     G_CALLBACK(OnGrabNotify), host_view);
    g_signal_connect(widget, "button-press-event",
                     G_CALLBACK(OnButtonPressReleaseEvent), host_view);
    g_signal_connect(widget, "button-release-event",
                     G_CALLBACK(OnButtonPressReleaseEvent), host_view);
    g_signal_connect(widget, "motion-notify-event",
                     G_CALLBACK(OnMouseMoveEvent), host_view);
    g_signal_connect(widget, "enter-notify-event",
                     G_CALLBACK(OnCrossingEvent), host_view);
    g_signal_connect(widget, "leave-notify-event",
                     G_CALLBACK(OnCrossingEvent), host_view);
    g_signal_connect(widget, "client-event",
                     G_CALLBACK(OnClientEvent), host_view);

    // Connect after so that we are called after the handler installed by the
    // WebContentsView which handles zoom events.
    g_signal_connect_after(widget, "scroll-event",
                           G_CALLBACK(OnMouseScrollEvent), host_view);
*/
    // Route calls to get_accessible to the view.
    // gtk_preserve_window_set_accessible_factory(
    //    GTK_PRESERVE_WINDOW(widget), GetAccessible, host_view);
    gtk_widget_realize(widget);
    gtk_widget_show(widget);
    return widget;
}

GtkWidget* CreateChild()
{

    return nullptr;
}




Window get_top_window(Display* d, Window start){
  Window w = start;
  Window parent = start;
  Window root = None;
  Window *children;
  unsigned int nchildren;
  Status s;

  printf("getting top window ... \n");
  while (parent != root) {
    w = parent;
    s = XQueryTree(d, w, &root, &parent, &children, &nchildren); // see man

    if (s)
      XFree(children);

    printf("  get parent (window: %d)\n", (int)w);
  }

  printf("success (window: %d)\n", (int)w);

  return w;
}

// GdkWindow* CreateRootWindow()
// {   
//     xdisplay_ =  XOpenDisplay(NULL);
//     if(!xdisplay_) {
//         cout << "x display create failed !" << endl;
//         return NULL;
//     }

//     XID  root = gdk_x11_get_default_root_xwindow(); 

//     Visual* visual = CopyFromParent;
//    // unsigned long attribute_mask = CWBackPixel | CWBitGravity;

//     XSetWindowAttributes swa;
//     memset(&swa, 0, sizeof(swa));
//     // swa.background_pixmap = 0;
//     swa.bit_gravity = NorthWestGravity;
//     swa.override_redirect = true;

//     swa.event_mask
//         = SubstructureRedirectMask // handle child window requests      (MapRequest)
//         | SubstructureNotifyMask   // handle child window notifications (DestroyNotify)
//         | StructureNotifyMask      // handle container notifications    (ConfigureNotify)
//         | ExposureMask             // handle container redraw           (Expose)
//         ;

//     swa.do_not_propagate_mask = 0; // do not hide any events from child window

//     swa.background_pixel = blue.pixel; // background color

//     unsigned long attribute_mask = CWEventMask  // enable attrs.event_mask
//                              | NoEventMask  // enable attrs.do_not_propagate_mask
//                              | CWBackPixel  // enable attrs.background_pixel






//   if (swa.override_redirect)
//     attribute_mask |= CWOverrideRedirect;

//     // XID  xwindow_ = XCreateWindow(xdisplay_, root, 448,
//     //                        406, 1584,
//     //                        2788,
//     //                        0,  // border width
//     //                        24, 1, visual, attribute_mask, &swa);
//     XID  xwindow_ = XCreateWindow(xdisplay_, root, 448,
//                            406, 1584,
//                            2788,
//                            0,  // border width
//                            24, 1, NULL, attribute_mask, &swa);


//     // GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//     // gtk_window_set_title (GTK_WINDOW (window), "Hello World");

//     // GtkWidget * button = gtk_button_new_with_label("Button");

//     // gtk_widget_realize(button);

//     // gtk_widget_show (window);

//     // cout << "window->window: " << GDK_WINDOW_XID(window->window) << endl;  

//    XMapWindow(xdisplay_, xwindow_);

//   Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", True);
//     // GdkWindow* widget_frame = gdk_x11_get_default_root_xwindow();
//     GdkWindow* widget_frame = gdk_window_foreign_new(xwindow_);
//     //gdk_window_show(widget_frame);
//     // GtkWidget* root_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     // gtk_window_set_title (GTK_WINDOW(root_window), "Window");
//     // gtk_widget_realize(root_window);

//     // Child window ID and its display
//     Display* child_display = NULL;
//     Window child_window = 0;

//     for (;;) {
//         XEvent event;
//         XNextEvent(xdisplay_, &event);

//         printf("container_event: %s\n", event_names[event.type]);

//         // Map child window when it requests and store its display and window id
//         if (event.type == MapRequest) {
//             XMapWindow(event.xmaprequest.display, event.xmaprequest.window);

//             child_display = event.xmaprequest.display;
//             child_window = event.xmaprequest.window;
//         }

//         // Propagate resize event to child window, and also resize it after MapRequest
//         if (event.type == ConfigureNotify || event.type == MapRequest) {
//             if (child_window) {
//                 // Get container window attributes
//                 XWindowAttributes attrs;
//                 XGetWindowAttributes(xdisplay_, xwindow_, &attrs);

//                 // Move and resize child
//                 XMoveResizeWindow(child_display,
//                                   child_window,
//                                   20, 20, attrs.width - 40, attrs.height - 40);
//             }
//         }

//         // Refresh container window
//         if (event.type == Expose) {
//             XClearWindow(xdisplay_, xwindow_);
//         }

//         // Exit if child window was destroyed
//         if (event.type == DestroyNotify) {
//             fprintf(stderr, "child window destroyed, exiting\n");
//             break;
//         }

//         // Close button
//         if (event.type == ClientMessage) {
//             if (event.xclient.data.l[0] == wm_delete) {
//                 break;
//             }
//         }
//     }




//     // gtk_widget_set_parent_window(root_window, widget_frame);
//     return widget_frame;
// }


void drawPixmap(GdkDrawable* pixmap, const char* filename, bool draw_circle){
  cairo_t* cairo = gdk_cairo_create(pixmap);
  if(draw_circle) {
    cairo_save(cairo);
    cairo_set_source_rgb(cairo, 1, 1, 0);
    cairo_rectangle(cairo, 0, 0, 500, 500);
    // cairo_stroke(cairo);
    cairo_fill(cairo);
    
    cairo_restore(cairo);
    cairo_save(cairo);
    cairo_set_line_width (cairo, 2.0);
    cairo_select_font_face(cairo, "Sans", CAIRO_FONT_SLANT_NORMAL,
        CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cairo, 40.0);
    cairo_move_to(cairo, 50.0, 50.0);
    cairo_show_text(cairo, "Disziplin ist Macht.");
    cairo_restore(cairo);
  }
  cairo_surface_t *cairo_surface = cairo_get_target (cairo);    
  cairo_surface_write_to_png (cairo_surface, filename);
  cairo_destroy(cairo);
}




