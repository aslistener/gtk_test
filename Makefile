atkcflags=`pkg-config --cflags atk`
gtkcflags=`pkg-config --cflags gtk+-2.0`
includeflags=-I. -Ixlib_test


atklflags=`pkg-config --libs atk`
gtklflags=`pkg-config  --libs gtk+-2.0`
x11lflags=-lx11 -llibXext

#objects=main.o gtk_preserve_window.o functions.o
sources= main.cc gtk_preserve_window.cc functions.cc \
  gtk_plugin_container.cc xlib_container.cc xlib_test/xshm.c \
	xlib_test/resources.c xlib_test/aligned_malloc.c xlib_test/thread_util.c

objs = main.o gtk_preserve_window.o functions.o \
  gtk_plugin_container.o xlib_container.o xshm.o \
	resources.o aligned_malloc.o thread_util.o

cc = g++

main:
	$(cc) -g $(atkcflags) $(gtkcflags) $(sources) -o main \
	  $(atklflags) $(gtklflags) -lX11 -lXext -lXt

# main:$(objs)
# 	$(cc)  $(atklflags) $(gtklflags) -lX11 -lXext $(objs) -o mainexe

# %.o: %.cc
# 	$(cc) -g $(atkcflags) $(gtkcflags) $(sources) -I. $^ -o $@ $(atklflags) $(gtklflags) -lX11 -lXext

clean:
	rm -f *.o mainexe