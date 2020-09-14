#include "gtk_window_test.h"

class RepaintTest{
  public:
  static void screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer user_data);
  static gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
  static void clicked(GtkWindow *win, GdkEventButton *event, gpointer user_data);
  static void realized (GtkWidget *widget, gpointer   user_data);

  static void child_realized (GtkWidget *widget, gpointer   user_data);
  static gboolean child_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);


public:
  RepaintTest(){  init();}
  void initPixmap();
  void init();
  void initSubWindow();
  ~RepaintTest(){     cairo_destroy(cr); }
public:
  static const int window_width;
  static const int window_height;
  static gboolean supports_alpha;
  static cairo_t *cr;
  static GtkWidget *window;
  static GtkWidget* button;
  static GtkWidget* child_window ;

  GdkPixmap *pixmap = 0;



};

gboolean RepaintTest::supports_alpha = FALSE;
cairo_t *RepaintTest::cr = 0;
GtkWidget *RepaintTest::window = 0;
GtkWidget* RepaintTest::button = 0;
GtkWidget* RepaintTest::child_window = 0;

const int RepaintTest::window_width = 800;
const int RepaintTest::window_height = 800;

void RepaintTest::init() 
{
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
  gtk_window_set_title(GTK_WINDOW(window), "Alpha Demo");
  g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);

  gtk_widget_set_app_paintable(window, TRUE);
  
  g_signal_connect(G_OBJECT(window), "realize", G_CALLBACK(RepaintTest::realized), this);
  g_signal_connect_after(G_OBJECT(window), "expose-event", G_CALLBACK(RepaintTest::expose), this);
  g_signal_connect(G_OBJECT(window), "screen-changed", G_CALLBACK(RepaintTest::screen_changed), NULL);
  //gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
  GtkWidget* fixed_container = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(window), fixed_container);
  button = gtk_button_new_with_label("button1");
  gtk_widget_set_size_request(button, 100, 100);
  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
  g_signal_connect(G_OBJECT(window), "button-press-event", G_CALLBACK(RepaintTest::clicked), this);
  gtk_container_add(GTK_CONTAINER(fixed_container), button);



  screen_changed(window, NULL, NULL);
  
  //initSubWindow();
 // gtk_widget_realize(window); 
  gtk_widget_show_all(window);
}

void RepaintTest::initSubWindow()
{
  child_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  // gtk_window_set_position(GTK_WINDOW(child_window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(child_window), window_width/2, window_height/2);

  //gtk_widget_set_app_paintable(child_window, TRUE);
  
   g_signal_connect(G_OBJECT(child_window), "realize", G_CALLBACK(RepaintTest::child_realized), this);
   g_signal_connect_after(G_OBJECT(child_window), "expose-event", G_CALLBACK(RepaintTest::expose), this);
  // g_signal_connect(G_OBJECT(window), "screen-changed", G_CALLBACK(RepaintTest::screen_changed), NULL);
 // gtk_window_set_decorated(GTK_WINDOW(child_window), FALSE);

   gtk_widget_set_app_paintable(child_window, TRUE);

  GtkWidget* fixed_container = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(child_window), fixed_container);
  GtkWidget* child_button = gtk_button_new_with_label("button2");
  gtk_widget_set_size_request(child_button, 100, 100);
  gtk_widget_add_events(child_button, GDK_BUTTON_PRESS_MASK);
  g_signal_connect(G_OBJECT(child_button), "button-press-event", G_CALLBACK(RepaintTest::clicked), NULL);
  gtk_container_add(GTK_CONTAINER(fixed_container), child_button);
   gtk_widget_show_all(child_window);

} 

void RepaintTest::initPixmap()
{
  pixmap = gdk_pixmap_new(window->window, 50, 50, -1);
  cairo_t *cairo = gdk_cairo_create(pixmap);
  cairo_set_source_rgba (cairo, 0.0, 0.0, 1.0, 1.0);
  cairo_set_operator (cairo, CAIRO_OPERATOR_SOURCE);
  cairo_rectangle(cairo, 0, 0, 50, 50);
  cairo_clip(cairo);
  cairo_paint (cairo);

  cairo_destroy(cairo);
}

void RepaintTest::screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata)
{
    /* To check if the display supports alpha channels, get the colormap */
    GdkScreen *screen = gtk_widget_get_screen(widget);
    GdkColormap *colormap = gdk_screen_get_rgba_colormap(screen);

    if (!colormap)
    {
        printf("Your screen does not support alpha channels!\n");
        colormap = gdk_screen_get_rgb_colormap(screen);
        supports_alpha = FALSE;
    }
    else
    {
        printf("Your screen supports alpha channels!\n");
        supports_alpha = TRUE;
    }

    gtk_widget_set_colormap(widget, colormap);
}

gboolean RepaintTest::expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
  PRINT_VAL(GDK_WINDOW_XID(widget->window));
    cr = gdk_cairo_create(widget->window);

    if (supports_alpha)
        cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0); /* transparent */
    else
        cairo_set_source_rgb (cr, 1.0, 0.0, 0.0); /* opaque white */

    /* draw the background */
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_rectangle(cr, 20, 20, 250, 250);
    cairo_clip(cr);
    cairo_paint (cr);


  //  GdkPixmap *src_pixmap = ((RepaintTest *)user_data)->pixmap;
  // cairo_t* src =  gdk_cairo_create(src_pixmap);
  // cairo_surface_t *image_surface = cairo_get_target(src);
  
  // cairo_t* dst =  gdk_cairo_create( RepaintTest::child_window->window);
  // cairo_set_source_surface(dst, image_surface, 0, 0);
  // cairo_set_operator (dst, CAIRO_OPERATOR_SOURCE);
  // cairo_paint(dst);
  // cairo_destroy(dst);
  // cairo_destroy(src);

  GdkDisplay* gdk_display = gdk_window_get_display(RepaintTest::window->window);
  Display* x_display = gdk_x11_display_get_xdisplay(gdk_display);
  
  cairo_surface_t *surface 
      = cairo_xlib_surface_create(x_display, GDK_WINDOW_XID(RepaintTest::child_window->window), CopyFromParent, 400, 400);
  cairo_t* dst =  cairo_create(surface);
  cairo_set_source_rgb(dst, 1.0, 1.0, 0.0);
  cairo_set_operator (dst, CAIRO_OPERATOR_SOURCE);
  cairo_paint(dst);
  cairo_destroy(dst);





//  XUngrabServer(x_display );
    //gtk_widget_queue_draw_area (GTK_WIDGET(RepaintTest::child_window), 0, 0, 40, 40);
    return FALSE;
}

void RepaintTest::clicked(GtkWindow *win, GdkEventButton *event, gpointer user_data)
{
  PRINT_VAL_MSG(event->button, "button ");
    /* toggle window manager frames */
    // gtk_window_set_decorated(win, !gtk_window_get_decorated(win));
    supports_alpha = !supports_alpha;
    //gtk_widget_show_all(window);
//    gtk_widget_queue_draw (GTK_WIDGET(win));
}

void RepaintTest::realized (GtkWidget *widget, gpointer   user_data)
{
    // gboolean has_window = gtk_widget_get_has_window(button);
    // PRINT_VAL_MSG(has_window, "%d", "button ");
    // has_window = gtk_widget_get_has_window(window);
    // PRINT_VAL_MSG(has_window, "%d", "window ");
    
  ((RepaintTest* )user_data)->initSubWindow();
  ((RepaintTest* )user_data)->initPixmap();

}

void RepaintTest::child_realized (GtkWidget *widget, gpointer   user_data)
{
  GdkDisplay* child_gdk_display = gdk_window_get_display(widget->window);
  Display* child_x_display = gdk_x11_display_get_xdisplay(child_gdk_display);
  XID widget_id = GDK_WINDOW_XID(widget->window);

  GdkDisplay* gdk_display = gdk_window_get_display(RepaintTest::window->window);
  Display* x_display = gdk_x11_display_get_xdisplay(gdk_display);

  //XGrabServer( child_x_display);
  XID widget_parent_id = GDK_WINDOW_XID(window->window);

  PRINT_VAL_MSG(widget_id, "child id is   ");
  PRINT_VAL_MSG(widget_parent_id, "parent id is   ");

  gdk_window_reparent(child_window->window, window->window, window_width/4, window_height/4);

  XSelectInput(x_display, widget_id, ExposureMask);

//  XReparentWindow(child_x_display, widget_id, widget_parent_id,  200, 200);
}

gboolean RepaintTest::child_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
  PRINT_VAL(GDK_WINDOW_XID(widget->window));
  // GdkPixmap *src_pixmap = ((RepaintTest *)user_data)->pixmap;
  // cairo_t* src =  gdk_cairo_create(src_pixmap);
  // cairo_surface_t *image_surface = cairo_get_target(src);
  

  // cairo_t* dst =  gdk_cairo_create(widget->window);
  // cairo_set_source_surface(dst, image_surface, 0, 0);
  // cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  // cairo_paint(dst);

  // cairo_destroy(dst);
  // cairo_destroy(src);


  // GdkGC *gc = gdk_gc_new(src_pixmap);
  // gdk_draw_drawable(widget->window, gc, src_pixmap, 0, 0, 0,0, 50, 50);
  // g_object_unref(gc);
}




void test_window_repaint() {

  RepaintTest repaint_test;  
  gtk_main ();
}