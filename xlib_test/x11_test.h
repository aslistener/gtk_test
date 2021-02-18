#pragma once

#include "functions.h"

// test for intereact between gtk and xlib 
namespace testx11 {
  int TestOpenDisplay();
  void test_clip();
  
  int test_double_buffer_clip ();
  
  void testClip();

  void testEvent();
  
  int test_main(int argc, char** argv);
  
  int test_gl_shaped_window(int argc, char *argv[]);
}