#include "x11_test.h"



   Display *d;
   Window w;

void ChildWindowRealizeCallback(GtkWidget *window, 
      GdkEvent *event, gpointer data) {

    GdkDisplay* gdkDisplay = gdk_window_get_display(window->window);
    Display* xDisplay = gdk_x11_display_get_xdisplay(gdkDisplay);
 
    //LOG(XConnectionNumber(d));
    PRINT_VAL(XConnectionNumber(xDisplay));
    PRINT_VAL(GDK_WINDOW_XID(gtk_widget_get_window(window)));

    XReparentWindow(xDisplay, GDK_WINDOW_XID(gtk_widget_get_window(window)), w, 400, 400);
}

int TestOpenDisplay() {
   int s;
   XEvent e;

                        /* open connection with the server */
   d=XOpenDisplay(NULL);
   if(d==NULL) {
     printf("Cannot open display\n");
     exit(1);
   }
   s=DefaultScreen(d);

    Colormap colormap = DefaultColormap(d, s);

    XColor blue;
    XAllocNamedColor(d, colormap, "blue", &blue, &blue);


    // Initialize container window attributes
    XSetWindowAttributes attrs;

    attrs.event_mask
        = SubstructureRedirectMask // handle child window requests      (MapRequest)
        | SubstructureNotifyMask   // handle child window notifications (DestroyNotify)
        | StructureNotifyMask      // handle container notifications    (ConfigureNotify)
        | ExposureMask             // handle container redraw           (Expose)
        ;

    attrs.do_not_propagate_mask = 0; // do not hide any events from child window

    attrs.background_pixel = blue.pixel; // background color

    unsigned long attrs_mask = CWEventMask  // enable attrs.event_mask
                             | NoEventMask  // enable attrs.do_not_propagate_mask
                             | CWBackPixel  // enable attrs.background_pixel
                        ;

                        /* create window */
  w=XCreateWindow(d, RootWindow(d, s),
            100,
            100,
            1000,
            1000,
            1,
            CopyFromParent,
            InputOutput,
            CopyFromParent,
            attrs_mask,
            &attrs);

   // Process Window Close Event through event handler so XNextEvent does Not fail
   Atom delWindow = XInternAtom( d, "WM_DELETE_WINDOW", 0 );
   XSetWMProtocols(d , w, &delWindow, 1);

                        /* select kind of events we are interested in */
   //XSelectInput(d, w, ExposureMask | KeyPressMask);

                        /* map (show) the window */
   XMapWindow(d, w);

   //  gtk_widget_get_window(button);
     
    //  GdkWindow* widget_frame = gdk_window_foreign_new(w);
    //  gtk_widget_set_parent_window(button, widget_frame);


  std::thread reparent_thread([&](){
  //  std::this_thread::sleep_for(5s);
    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    // gtk_window_set_title (GTK_WINDOW (window), "Hello World");
    GtkWidget * button = gtk_button_new_with_label("Button");
    gtk_widget_show(button);

    gtk_container_add(GTK_CONTAINER(window), button);

    g_signal_connect(G_OBJECT(window), "realize",
          G_CALLBACK(ChildWindowRealizeCallback), (gpointer)(&w));

    //gtk_widget_realize(button);
    gtk_widget_show_all(window);
    gtk_main();
  });
  reparent_thread.detach();

    Display* child_display = NULL;
    Window child_window = 0;

    Atom wm_delete = XInternAtom(d, "WM_DELETE_WINDOW", True);
    // Subscribe WM_DELETE_WINDOW message
    XSetWMProtocols(d, w, &wm_delete, 1);

    //     gtk_widget_show (button);
    
    /* event loop */
    for (;;) {
        XEvent event;
        XNextEvent(d, &event);

        PRINT_VAL(event_names[event.type]);

        // Map child window when it requests and store its display and window id
        if (event.type == MapRequest) {
            XMapWindow(event.xmaprequest.display, event.xmaprequest.window);

            child_display = event.xmaprequest.display;
            child_window = event.xmaprequest.window;

            if (child_window) {
                // Get container window attributes
                XWindowAttributes attrs;
                XGetWindowAttributes(d, w, &attrs);

                // Move and resize child
                XMoveResizeWindow(child_display,
                                  child_window,
                                  100, 100, attrs.width - 200, attrs.height - 200);
            }
        }
        // Propagate resize event to child window, and also resize it after MapRequest
        else if (event.type == ConfigureNotify) {
            if (child_window) {
                // Get container window attributes
                XWindowAttributes attrs;
                XGetWindowAttributes(d, w, &attrs);

                // Move and resize child
                XMoveResizeWindow(child_display,
                                  child_window,
                                  100, 100, attrs.width - 200, attrs.height - 200);
            }
        }

        // Refresh container window
        // else if (event.type == Expose) {
        //     XClearWindow(d, w);
        // }

        // Exit if child window was destroyed
        else if (event.type == DestroyNotify) {
            fprintf(stderr, "child window destroyed, exiting\n");
            break;
        }

        // Close button
        else if (event.type == ClientMessage) {
            if (event.xclient.data.l[0] == wm_delete) {
                break;
            }
        }
    }
                        /* destroy our window */
   XDestroyWindow(d, w);

                        /* close connection to server */
   XCloseDisplay(d);

   return 0;
 }