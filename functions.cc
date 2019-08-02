#include "functions.h"

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

GtkWidget* CreateRootWindow()
{   
    xdisplay_ = XOpenDisplay(NULL);
    if(!xdisplay_) {
        cout << "x display create failed !" << endl;
        return NULL;
    }

    Visual* visual = CopyFromParent;
    unsigned long attribute_mask = CWBackPixel | CWBitGravity;

    XSetWindowAttributes swa;
    memset(&swa, 0, sizeof(swa));
    swa.background_pixmap = 0;
    swa.bit_gravity = NorthWestGravity;
    swa.override_redirect = true;

  if (swa.override_redirect)
    attribute_mask |= CWOverrideRedirect;

    XID  xwindow_ = XCreateWindow(xdisplay_, NULL, 448,
                           406, 1584,
                           2788,
                           0,  // border width
                           24, 1, visual, attribute_mask, &swa);

    GdkWindow* widget_frame = gdk_window_foreign_new(xwindow_);

    GtkWidget* root_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(root_window), "Window");
    gtk_widget_realize(root_window);
    
    gtk_widget_set_parent_window(root_window, widget_frame);
    return root_window;
}