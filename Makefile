atkcflags=`pkg-config --cflags atk`
gtkcflags=`pkg-config --cflags gtk+-2.0`
includeflags=-I. -Ixlib_test


atklflags=`pkg-config --libs atk`
gtklflags=`pkg-config  --libs gtk+-2.0`
x11lflags= -lX11 -lXext -lXt -ljpeg

#objects=main.o gtk_preserve_window.o functions.o
sources= gtk_preserve_window.cc functions.cc \
  gtk_plugin_container.cc xlib_container.cc xlib_test/xshm.c \
	xlib_test/resources.c xlib_test/aligned_malloc.c xlib_test/thread_util.c bitmap_test.cc x11_test.cc   main.cc 

objs = gtk_preserve_window.o functions.o \
  gtk_plugin_container.o xlib_container.o xlib_test/xshm.o \
	xlib_test/resources.o xlib_test/aligned_malloc.o xlib_test/thread_util.o bitmap_test.o x11_test.o  main.o

cc = g++

main:
	$(cc) -g $(atkcflags) $(gtkcflags) $(sources) -o main \
	  $(atklflags) $(gtklflags) $(x11lflags)

# main:$(objs)
# 	$(cc)  $(atklflags) $(gtklflags) $(x11lflags) $(objs) -o main

# %.o: %.cc
# 	$(cc) -g $(atkcflags) $(gtkcflags) -I. $% -o $@ $(atklflags) $(gtklflags) $(x11lflags)

# %.o: %.c
# 	$(cc) -g $(atkcflags) $(gtkcflags) -I. $% -o $@ $(atklflags) $(gtklflags) $(x11lflags)

clean:
	rm -f *.o main