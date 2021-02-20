#include "x11_object_test.h"
#include "x11_objects_methods.h"

namespace testx11 {
  class PaintX11Window:
      public internal::CustomX11WindowBase {
  public:
    typedef struct{
      int id; 
      XRectangle rec;
    } Rectangle;
    PaintX11Window();
    ~PaintX11Window() override;

  protected:
    void HandleExposeEvent(const XEvent &event) override;
    void HandleMapRequestEvent(const XEvent &event) override;
    void HandleConfigureEvent(const XEvent &event) override;

  private:
    void createPixmap();
    void create8BitPixmap();

    void showRectangle(Rectangle rectangle);
    void hideRectangle(Rectangle rectangle);
    void moveRectangle(int width, int height);
    void calClip(Rectangle rectangle, bool is_clip);

  private:
      Rectangle rectangle_;
      // std::map<int, XRectangle> rectangles_;
      unsigned char* mask_data_ = 0;
      Pixmap pixmap_ = 0;
  };

  PaintX11Window::PaintX11Window() {}
  PaintX11Window::~PaintX11Window() {}

  void PaintX11Window::
      HandleExposeEvent(const XEvent &event) {
    // XDrawRectangle(display_, child_window_, gc_, 100, 100, 200, 200);
    if(child_window_)
      XDrawRectangle(display_, child_window_, gc_, 100, 100, 200, 200);
    if(event.xexpose.window == child_window_) 
    {
        XDrawRectangle(child_display_, child_window_, gc_, 100, 100, 200, 200);
    }
    // XDrawRectangle(display_, window_, gc_, 0, 0, 200, 200);
    // if(event.xexpose.window == child_window) {
    //      cairo_surface_t *surface 
    //      = cairo_xlib_surface_create(d, child_window, CopyFromParent, 400, 400);
    //  cairo_t* dst =  cairo_create(surfa
    
    // cairo_t *src = gdk_cairo_create(pixmap);
    // cairo_set_source_surface(dst, cairo_get_target(src), 0,
    // //cairo_set_source_rgb(dst, 1.0, 1.0, 0.0);
    // cairo_set_operator (dst, CAIRO_OPERATOR_SOURCE);
    // cairo_paint(dst);
    // cairo_destroy(dst);
    // //XSync(d, 0);
    // }
  }

  void PaintX11Window::
      HandleConfigureEvent(const XEvent &event) {
    if (child_window_) {
      // Get container window attributes
      XWindowAttributes attrs;
      XGetWindowAttributes(display_, window_, &attrs);
      // Move and resize child
      XMoveResizeWindow(child_display_,
                        child_window_,
                        100, 100, attrs.width - 200, attrs.height - 200);
      gc_ = XCreateGC(child_display_, child_window_, 0, 0);
      XSetBackground (display_, gc_, BlackPixel (display_, DefaultScreen(display_)));
    }
  }
  
  void PaintX11Window::
      HandleMapRequestEvent(const XEvent &event) {
    XMapWindow(event.xmaprequest.display, event.xmaprequest.window);
    child_display_ = event.xmaprequest.display;
    child_window_ = event.xmaprequest.window;
    PRINT_VAL(child_window_);
    PRINT_VAL(window_);
    if (child_window_) 
    {
      // Get container window attributes
      XWindowAttributes attrs;
      XGetWindowAttributes(display_, window_, &attrs);
      // Move and resize child
      XMoveResizeWindow(child_display_,
                        child_window_,
                        100, 100, attrs.width - 200, attrs.height - 200);
      XSelectInput(display_, child_window_, ExposureMask);
      std::vector<unsigned long> window_ids;
      EnumerateWindows(child_display_, child_window_, 1, 1, &window_ids);
      std::for_each(window_ids.begin(), window_ids.end(), [=](unsigned long id){
        XSelectInput(display_, id, ExposureMask);
      });
    }
  }

  void PaintX11Window::createPixmap(){
    // return create8BitPixmap()
    static unsigned char b = 0;
    b = (b==0? 0XFF: 0);
    int width  = 400; //window_width_/8+1
    mask_data_ = new unsigned char[width*window_height_];
    memset(mask_data_, 0xFF, width * window_height_);
    for(int i = 100; i < 200; i++)
      for(int j = 15; j < 25; j+=1)
        mask_data_[i*width + j] = b;
        
    // pixmap_  = XCreateBitmapFromData(display_, window_,
    //       (char*)mask_data_ , width*8, window_height_);    
    // XShapeCombineMask(display_, window_, ShapeBounding, 0, 0, pixmap_, ShapeSet);
  
    for(int i = 100; i < 200; i++)
    for(int j = 30; j < 40; j+=1)
      mask_data_[i*width + j] = b;
    if(pixmap_){
        XFreePixmap(display_, pixmap_);
        pixmap_ = 0;
      
      pixmap_  = XCreateBitmapFromData(display_, window_,
            (char*)mask_data_ , width*8, window_height_);    
          XShapeCombineMask(display_, window_, ShapeBounding, 0, 0, pixmap_, ShapeSet);    
    }
  }

  void PaintX11Window::create8BitPixmap(){
    /* convenience */
    int BLACK_PIXEL = BlackPixel(display_, DefaultScreen(display_));
    int WHITE_PIXEL = WhitePixel(display_, DefaultScreen(display_)); 
    XGCValues shape_xgcv;
    shape_xgcv.background = BLACK_PIXEL;
    shape_xgcv.foreground = WHITE_PIXEL;
    int width = window_width_/4;
    int height = window_height_/4;
    Pixmap pmap = XCreatePixmap(child_display_, child_window_, width, height, 1);
    GC shape_gc = XCreateGC(child_display_, pmap, GCForeground | GCBackground, &shape_xgcv);
    
    XSetForeground(child_display_, shape_gc, WHITE_PIXEL);
    XFillRectangle(child_display_, pmap, shape_gc, 0, 0, width, height);  
    
    XSetForeground(child_display_, shape_gc, BLACK_PIXEL);
    XFillRectangle(child_display_, pmap, shape_gc, 100, 100, 100, 100);  
    XShapeCombineMask (child_display_, child_window_, ShapeBounding,
            0, 0, pmap, ShapeSet);
  } 

  void testClip() {
    PaintX11Window x11_test;
    //x11_test.queryShapeExtension();
    //x11_test.createPixmap();
    PaintX11Window::handleEvents(&x11_test) ;
  }
}