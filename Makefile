atkcflags=`pkg-config --cflags atk`
gtkcflags=`pkg-config --cflags gtk+-2.0`

atklflags=`pkg-config --libs atk`
gtklflags=`pkg-config  --libs gtk+-2.0`
x11lflags=-lx11

#objects=main.o gtk_preserve_window.o functions.o
sources= main.cc gtk_preserve_window.cc functions.cc gtk_plugin_container.cc
objs = main.o gtk_preserve_window.o functions.o gtk_plugin_container.o

cc = g++

main:
	$(cc) -g $(atkcflags) $(gtkcflags) $(sources) -o main  $(atklflags) $(gtklflags) -lX11

#main:$(objs)
#	$(cc)  $(atklflags) $(gtklflags) $(objs) -o main
#
#%.o: %.cc
#	$(cc) -g $(atkcflags) $(gtkcflags) $(sources) -I. $^ -o $@ $(atklflags) $(gtklflags)

clean:
	rm -f *.o main