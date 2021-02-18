#include "bitmap_test.h"

#include "xlib_test/xshm.h"


class BitmapTest{
public:
  BitmapTest(){
    xdisplay = GDK_DISPLAY();
    GdkVisual* sys_visual = gdk_visual_get_system();

    pixmap = gdk_pixmap_new(NULL,  // use width/height/depth params
                            1000, 1000,
                            sys_visual->depth);
    // TODO(erg): Replace this with GdkVisual when we move to GTK3.
    GdkColormap* colormap = gdk_colormap_new(gdk_visual_get_system(),
                                            FALSE);
    gdk_drawable_set_colormap(pixmap, colormap);
    // unsigned long opacity = (0xffffffff / 0xff) * 1.0;
    // Atom property = XInternAtom(xdisplay, "_NET_WM_WINDOW_OPACITY", false);
    // if (property != None) {
    //   XChangeProperty(xdisplay, pixmap, property, 
    //                   XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&opacity, 1);
    //   XFlush(xdisplay);
    // }
  }

  ~BitmapTest(){
    gdk_pixmap_unref(pixmap);
  }

  void clearBitmap();
  void paintBitmap();
  static void paintMask(GdkWindow* window);
  void paintMask();

  void infoOfPixmap();

  void createGtkWidget();


  void saveCairoToFile(cairo_surface_t* s);

  GdkDrawable* pixmap;
  Display *xdisplay;
  GtkWidget *window;
  GtkWidget *child;
};

static gboolean paint (GtkWidget      *widget,
       GdkEventExpose *eev,
       gpointer        data) {
  BitmapTest::paintMask(widget->window); 
  return FALSE;
}

void BitmapTest::createGtkWidget() {
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Hello World");
    child = gtk_button_new_with_label("Button");
    gtk_widget_set_size_request(child, 50, 20);
    gtk_container_add(GTK_CONTAINER(window), child);

    gtk_widget_set_app_paintable(child, TRUE);
    g_signal_connect(G_OBJECT(child), "expose-event",
                     G_CALLBACK (paint),NULL);

    g_signal_connect(G_OBJECT(window),
        "destroy", gtk_main_quit, NULL);

    gtk_widget_set_size_request(window, 500, 500);

    gtk_widget_show_all(window);
       
    gtk_main ();
}

void BitmapTest::infoOfPixmap() {
  

}

void BitmapTest::paintMask(GdkWindow* window) {
  // cairo_surface_t *surface;
  // cairo_t *cr;

  // surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 500, 500);
  // cr = cairo_create (surface);


  // int              w, h;
  // cairo_surface_t *image;

  // cairo_arc (cr, 128.0, 128.0, 76.8, 0, 2*3.14);
  // cairo_clip (cr);
  // cairo_new_path (cr); /* path not consumed by clip()*/

  // image = cairo_image_surface_create_from_png ("/home/llq/Documents/aura-arch4.png");
  // w = cairo_image_surface_get_width (image);
  // h = cairo_image_surface_get_height (image);

  // cairo_scale (cr, 256.0/w, 256.0/h);

  // cairo_set_source_surface (cr, image, 0, 0);
  // cairo_paint (cr);



  // cairo_surface_t *surface = gdk_window_create_similar_surface(window, 
  //             CAIRO_CONTENT_COLOR_ALPHA, 500, 500); 
  // cairo_t *cr2 = cairo_create (surface);


   cairo_t* cr2 = gdk_cairo_create(window);

  cairo_save (cr2);
  cairo_set_font_size (cr2, 40);
  cairo_move_to (cr2, 40, 60);
  cairo_set_source_rgb (cr2, 0,1,0);
  cairo_show_text (cr2, "Hello World");
  cairo_restore (cr2);

   //cairo_set_source_surface (cr2, cairo_get_target(cr), 0, 0);
  //  cairo_set_source_rgb (cr2, 0, 0, 1);
  //  cairo_arc (cr2, 128.0, 128.0, 76.8, 0, 2*3.14);
  // // cairo_stroke(cr2);
  //  cairo_clip (cr2);
  //  cairo_fill(cr2);
  //cairo_new_path (cr2); /* path not consumed by clip()*/

//  cairo_paint (cr2);

  // saveCairoToFile(cairo_get_target(cr2));

  // cairo_destroy (cr);
  cairo_destroy (cr2);

}

void BitmapTest::paintMask() {
//cairo_t* cr = gdk_cairo_create(pixmap);

cairo_surface_t *surface;
cairo_t *cr;

surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 500, 500);
cr = cairo_create (surface);


int              w, h;
cairo_surface_t *image;

cairo_arc (cr, 128.0, 128.0, 76.8, 0, 2*3.14);
cairo_clip (cr);
cairo_new_path (cr); /* path not consumed by clip()*/

image = cairo_image_surface_create_from_png ("/home/llq/Documents/aura-arch4.png");
w = cairo_image_surface_get_width (image);
h = cairo_image_surface_get_height (image);

cairo_scale (cr, 256.0/w, 256.0/h);

cairo_set_source_surface (cr, image, 0, 0);
cairo_paint (cr);


cairo_t* cr2 = gdk_cairo_create(pixmap);
cairo_set_source_surface (cr2, cairo_get_target(cr), 0, 0);
cairo_paint (cr2);

saveCairoToFile(cairo_get_target(cr2));

cairo_destroy (cr);
}

void BitmapTest::clearBitmap(){
  cairo_t* cairo = gdk_cairo_create(pixmap);

  ///////////////
  PRINT_VAL_MSG("","not implement");
  ////////////////

  cairo_surface_t* s  = cairo_get_target(cairo);
  saveCairoToFile(s);
  cairo_destroy(cairo);
}



void BitmapTest::paintBitmap(){
  cairo_t* cairo = gdk_cairo_create(pixmap);
  
  cairo_save(cairo);
  cairo_set_source_rgba(cairo, 1, 1, 1, 0);
  cairo_rectangle(cairo, 0, 0, 500, 500);
  //cairo_stroke(cairo);
  //cairo_clip(cairo);
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

  
  

  cairo_surface_t* s  = cairo_get_target(cairo);
  saveCairoToFile(s);
  cairo_destroy(cairo);
} 

char filename[100] = {0};
const char filename_format[] = "/code/tmppngs/cairo_%d_%d.png";
void BitmapTest::saveCairoToFile(cairo_surface_t* s) {
  static int i = 1;
  i++;
  memset(filename, 0, 100);
  sprintf(const_cast<char* >(filename), filename_format, 0, i);
  cairo_surface_write_to_png (s, filename);
}


void test_bitmap() {
  BitmapTest t;
  t.createGtkWidget();
}


int save_ximage_pnm(XImage *img,char *pnmname,int type) {
    // int x,y;
    // unsigned long pixel;
    // FAIL_ON(!img || !pnmname || type<=0, "bad argument(s)"); 

    // fprintf(stdout,"P%d\n%d %d\n255\n",type,img->width, img->height);
    // for (y=0; y<img->height; y++) {
    //  for (x=0; x<img->width; x++) {
    //      pixel=XGetPixel(img,x,y);
    //      if (type==ASCII_PPM) {
    //          fprintf(stdout,"%ld %ld %ld\n",
    //                  pixel>>16,(pixel&0x00ff00)>>8,pixel&0x0000ff);
    //      } else if (type==BINARY_PPM) {
    //          fprintf(stdout,"%c%c%c",
    //              (char)(pixel>>16),
    //              (char)((pixel&0x00ff00)>>8),
    //              (char)(pixel&0x0000ff) );

    //      } else {
    //          fprintf(stderr,"PnM type %d not supported!",type);
    //          return(0);
    //      }
    //  }
    // }
    return(1);
}

int write_jpeg(XImage *img, const char* filename)
{
    FILE* outfile;
    unsigned long pixel;
    int x, y;
    char* buffer;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;
    JSAMPROW row_pointer;

    outfile = fopen(filename, "wb");
    if (!outfile) {
        fprintf(stderr, "Failed to open output file %s", filename);
        return 1;
    }

    /* collect separate RGB values to a buffer */
    buffer = (char *)malloc(sizeof(char)*3*img->width*img->height);
    for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            pixel = XGetPixel(img,x,y);
            buffer[y*img->width*3+x*3+0] = (char)(pixel>>16);
            buffer[y*img->width*3+x*3+1] = (char)((pixel&0x00ff00)>>8);
            buffer[y*img->width*3+x*3+2] = (char)(pixel&0x0000ff);
        }
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = img->width;
    cinfo.image_height = img->height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 85, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer = (JSAMPROW) &buffer[cinfo.next_scanline
                      *(img->depth>>3)*img->width];
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }
    free(buffer);
    jpeg_finish_compress(&cinfo);
    fclose(outfile);

    return 0;
}


void TestPaintSharedImageRealizeCallback(GtkWidget *window, 
      GdkEvent *event, gpointer data) {
  GdkDisplay* gdk_display = gdk_window_get_display(window->window);
  Display* x_display = gdk_x11_display_get_xdisplay(gdk_display);
  
  auto createXPixmap = [](GdkDrawable *drawable){
    GdkVisual* sys_visual = gdk_visual_get_system();
    GdkDrawable* pixmap_ = gdk_pixmap_new(drawable,  // use width/height/depth params
                           500, 500,
                           sys_visual->depth);
    // TODO(erg): Replace this with GdkVisual when we move to GTK3.
    GdkColormap* colormap = gdk_colormap_new(gdk_visual_get_system(),
                                             FALSE);
    gdk_drawable_set_colormap(pixmap_, colormap);
    // The GdkDrawable now owns the GdkColormap.
    g_object_unref(colormap);
    
    return pixmap_;
  };  

  GtkWidget* drawing_area = gtk_drawing_area_new(); 
  gtk_drawing_area_size((GtkDrawingArea *)drawing_area, 500, 500); 
  auto* image = gtk_image_new();
  gtk_widget_realize(image); 


  GdkDrawable* rootwindow_pixmap = createXPixmap(NULL);
  GdkDrawable* window_pixmap = createXPixmap(window->window);
  GdkDrawable* image_pixmap = createXPixmap(image->window);
  GdkDrawable* drawing_area_pixmap = createXPixmap(drawing_area->window);

  auto createHexFile = [](GdkDrawable* pixmap, const char* filename){


  };

  bool draw_circle = true;

  // drawPixmap(rootwindow_pixmap, "/code/tmppngs/rootwindow_pixmap.png", draw_circle);
  // drawPixmap(window_pixmap, "/code/tmppngs/window_pixmap.png", draw_circle);
  // drawPixmap(image_pixmap, "/code/tmppngs/image_pixmap.png", draw_circle);
  // drawPixmap(drawing_area_pixmap, "/code/tmppngs/drawing_area_pixmap.png", draw_circle);

  // // int shm_id = * (int*)data;
  // XShmSegmentInfo shminfo;
  // memset(&shminfo, 0, sizeof(shminfo));
  // shminfo.shmid = g_xshm_id;
  // // This function is only called if QuerySharedMemorySupport returned true. In
  // // which case we've already succeeded in having the X server attach to one of
  // // our shared memory segments.
  // if (!XShmAttach(x_display, &shminfo)) {
  //   PRINT_VAL_MSG(shminfo.shmid, "X failed to attach to shared memory segment ");
  //   x_shm_id = 0;
  // } else {
  //   PRINT_VAL_MSG(shminfo.shmid, "X attached to shared memory segment ");
  // }
  
  // XImage image;
  
  // get_xshm_image(x_display, GDK_WINDOW_XID(window->window), &image, 100, 100, ~0L, &shminfo);
//  write_jpeg(&image, "/code/tmppngs/2222.jpeg");
} 

int g_xshm_id = 0;
void TestPaintSharedImage(int xshm_id) {
  g_xshm_id = xshm_id;
    // auto *window = gtk_offscreen_window_new ();

    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 1000);

    gtk_window_set_title (GTK_WINDOW (window), "Hello World");
    GtkWidget * button = gtk_button_new_with_label("Button");
    gtk_widget_show(button);

    gtk_container_add(GTK_CONTAINER(window), button);

    g_signal_connect(G_OBJECT(window), "realize",
          G_CALLBACK(TestPaintSharedImageRealizeCallback), (gpointer)(&xshm_id));

    // gtk_signal_connect (GTK_OBJECT (window), "destroy",
    //                     GTK_SIGNAL_FUNC (destroy), NULL);

    //gtk_widget_realize(button);
    gtk_widget_show_all(window);
    gtk_main();
}


int x_shm_id = 0;
void test_xshm(int shm_id) {

  if(shm_id == 0) {
    Display* x_display_1 = XOpenDisplay(NULL); 
    Visual* x_visual = CopyFromParent;
    // PRINT_VAL(XShmQueryExtension (x_display_1));  
    
    // int major = 0, minor = 0, pixmap = 0;
    // PRINT_VAL( XShmQueryVersion(x_display_1, &major, &minor, &pixmap ) );
    // PRINT_VAL(major);
    // PRINT_VAL(minor);
    // PRINT_VAL(pixmap);
    // PRINT_VAL(XShmPixmapFormat(x_display_1));

    XShmSegmentInfo x_shm_info;
    create_xshm_image(x_display_1, x_visual, CopyFromParent, DefaultDepth(x_display_1, DefaultScreen(x_display_1)),
      &x_shm_info, 500, 500);
      x_shm_id = x_shm_info.shmid;
  } else {
    TestPaintSharedImage(shm_id);
    x_shm_id = 0;
  }

}

#include <bitset>
#define xlogo16_width 16
#define xlogo16_height 16
void test_mask()
{

  unsigned char xlogo16_bits[] = {
     0x0f, 0x80, 0x1e, 0x80, 0x3c, 0x40, 0x78, 0x20, 0x78, 0x10, 0xf0, 0x08,
     0xe0, 0x09, 0xc0, 0x05, 0xc0, 0x02, 0x40, 0x07, 0x20, 0x0f, 0x20, 0x1e,
     0x10, 0x1e, 0x08, 0x3c, 0x04, 0x78, 0x02, 0xf0};

    // auto printChar = [](char x){
    //   cout << int(x & 0x80)
    //   << (x & 0x40)
    //   << (x & 0x20)
    //   << (x & 0x10)
    //   << (x & 0x08)
    //   << (x & 0x04)
    //   << (x & 0x02)
    //   << (x & 0x01);
    // };
  for(int i = 0; i< 16; i+=2)
  {
    cout << bitset<8>(xlogo16_bits[i]) ;
    cout << bitset<8>(xlogo16_bits[i+1]) <<endl;
    // printChar(xlogo16_bits[i]);
    // printChar(xlogo16_bits[i+1]);
    //cout << endl;
  }
}