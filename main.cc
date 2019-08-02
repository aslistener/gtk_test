#include <assert.h>
#include "functions.h"

class RAIIResource {
 public:
   RAIIResource(){}
   ~RAIIResource(){}   
};



void configure_callback(GtkWindow *window, 
      GdkEvent *event, gpointer data) {
          
   int x, y;
   GString *buf;
   
   x = event->configure.x;
   y = event->configure.y;
   
   buf = g_string_new(NULL);   
   g_string_printf(buf, "%d, %d, %d, %d", x, y,
      event->configure.width,
      event->configure.height);
   
   gtk_window_set_title(window, buf->str);
   
   g_string_free(buf, TRUE);
}

void TestContainer()
{



}


void TestPreserved()
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *halign1, *halign2;

  //window = CreatePreservedChild();
  GtkWidget *preserve = CreatePreservedChild();
  //  GtkWidget *preserve = content::gtk_plugin_container_new();
  window = CreateRootWindow();
  //gtk_window_new (GTK_WINDOW_TOPLEVEL);
//   g_signal_connect (window, "delete_event", gtk_main_quit, NULL); /* dirty */

//   view = create_view_and_model ();

  // sets some border space around the edges of the window. 
   gtk_container_set_border_width(GTK_CONTAINER(window), 15);

   button = gtk_button_new_with_label("Button");


  // The GtkAlignment is a basic container 
  // which can be used to align its child to the sides of the window. 
  // In our case, the button is placed to the upper-left corner of the window. 
  // The first parameters of the function are the xalign and yalign. 
  // A value of 0 for xalign indicates left alignment; 
  // a value of 0 for yalign indicates top alignment. 
  // The third and fourth parameters are scaling values.
  // Passing 0 to both parameters indicates that the widget does not expand in both directions. 
  //halign1 = gtk_alignment_new(0, 0, 0, 0);
  //halign2 = gtk_alignment_new(0, 20, 0, 0);
  //gtk_container_add (GTK_CONTAINER (halign), preserve);

  gtk_container_add (GTK_CONTAINER (preserve), button);
  gtk_preserve_window_set_preserve((GtkPreserveWindow *)preserve, true);

  //gtk_container_add (GTK_CONTAINER (preserve), halign1);
  //gtk_container_add(GTK_CONTAINER(preserve), halign1);
  gtk_container_add(GTK_CONTAINER(window), preserve);
  gtk_widget_add_events(GTK_WIDGET(window), GDK_CONFIGURE);
  g_signal_connect(G_OBJECT(window), "configure-event",
        G_CALLBACK(configure_callback), NULL);

  gtk_widget_realize(preserve);
  gtk_widget_show(preserve);
  gtk_widget_show(button);

  GdkWindow* gdk_window = gtk_widget_get_window(preserve);
  assert(gdk_window);
  

  // gtk_widget_add_events(GTK_WIDGET(gtk_widget_get_window(preserve)), GDK_CONFIGURE);
  // g_signal_connect(G_OBJECT(preserve), "configure-event",
  // G_CALLBACK(configure_callback), NULL);
  //assert(GTK_WIDGET(gdk_window) == window);

  g_signal_connect(window, "destroy",
      G_CALLBACK(gtk_main_quit), NULL); 
  //gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
  gtk_widget_show_all (window);


}





int main(int argc, char ** argv)
{


  gtk_init (&argc, &argv);

  TestPreserved();


  gtk_main ();

  XCloseDisplay(xdisplay_);
  return 0;

}

