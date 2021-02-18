# cross compiler
CROSS=aarch64-linux-gnu
cc=$(CROSS)-gcc
cxx=$(CROSS)-g++
linker=$(CROSS)-ld
#cc=gcc
#cxx=g++
#linker=gcc

atkcflags=`pkg-config --cflags atk`
gtkcflags=`pkg-config --cflags gtk+-2.0`
includeflags=-I. -Ixlib_test


atklflags=`pkg-config --libs atk`
gtklflags=`pkg-config  --libs gtk+-2.0`
gdkpixbuflflags=`pkg-config --libs gdk-pixbuf-xlib-2.0`
x11lflags= -lX11 -lXext -lXt -ljpeg -L. -Lxlib_test $(gdkpixbuflflags) -lGL -lXi

cpp_sources= $(wildcard *.cc xlib_test/*.cc)
c_sources = $(wildcard *.c xlib_test/*.c)

cpp_objs=$(patsubst %.cc,%.o,$(cpp_sources) )
c_objs= $(patsubst %.c,%.o,$(c_sources) )

objs   = $(patsubst %.cc,%.o,$(cpp_sources) )  $(patsubst %.c,%.o,$(c_sources) )

CC = g++

CFLAGS =  -g $(atkcflags) $(gtkcflags) -z,max-page-size=65536
CPPFLAGS =  -g $(atkcflags) $(gtkcflags) -z,max-page-size=65536

main: $(c_objs) $(cpp_objs)
	$(CC) -g $(atkcflags) $(gtkcflags) -o main $(cpp_sources) $(c_sources)  \
	  $(atklflags) $(gtklflags) $(x11lflags)


# main: $(c_objs) $(cpp_objs)
# 	$(cc)  -g -o main $(c_objs) $(cpp_objs) \
# 	  $(atklflags) $(gtklflags) $(x11lflags);

# $(cpp_objs): $(cpp_sources)
# 	$(cc) -g $(atkcflags) $(gtkcflags) -I. $< -o $@ $(atklflags) $(gtklflags) $(x11lflags)


print:
	@echo $(objs)

clean:
	rm -f *.o main