# cross compiler
# CROSS=aarch64-linux-gnu
# cc=$(CROSS)-gcc
# cxx=$(CROSS)-g++
# linker=$(CROSS)-ld

cc=gcc
cxx=g++
linker=gcc

atkcflags=`pkg-config --cflags atk`
gtkcflags=`pkg-config --cflags gtk+-2.0`
includeflags=-I. -Ixlib_test -Igtk_test -Imixed_oop


atklflags=`pkg-config --libs atk`
gtklflags=`pkg-config  --libs gtk+-2.0`
gdkpixbuflflags=`pkg-config --libs gdk-pixbuf-xlib-2.0`
x11lflags= -lX11 -lXext -lXt -ljpeg -L. -Lxlib_test -Lgtk_test $(gdkpixbuflflags) -lGL -lXi

cpp_sources=$(wildcard xlib_test/*.cc gtk_test/*.cc mixed_oop/*.cc *.cc)

c_sources=$(wildcard xlib_test/*.c gtk_test/*.c)

cpp_objs=$( patsubst %.cc,%.o,$(cpp_sources) )
c_objs=$(patsubst %.c,%.o,$(c_sources) )

objs   = $(patsubst %.cc,%.o,$(cpp_sources) )  $(patsubst %.c,%.o,$(c_sources) )

# objs=$(patsubst %.cc,build/%.o,$(notdir $(cpp_sources) ) ) \
# 		$(patsubst %.c,build/%.o,$(notdir $(c_sources) ) )

CC=g++

CFLAGS= -g $(atkcflags) $(gtkcflags)  $(includeflags)   #-z,max-page-size=65536
CPPFLAGS= -g $(atkcflags) $(gtkcflags)   $(includeflags) #-z,max-page-size=65536

build/main:$(objs)
	$(CC) -g $(atkcflags) $(gtkcflags) $(includeflags) \
	-o build/main $(cpp_sources) $(c_sources)  \
	$(atklflags) $(gtklflags) $(x11lflags)


# build/main:$(objs)
# 	$(CC) -e entry main.cc $(CPPFLAGS) -g -o build/main $(objs) \
# 	  $(atklflags) $(gtklflags) $(x11lflags)

# $(objs): $(cpp_sources) $(c_sources)
# 	$(CC) -g -c $(atkcflags) $(gtkcflags)  $(includeflags) $< -o $@ \
# 	$(atklflags) $(gtklflags) $(x11lflags)

# $(objs):$(cpp_sources) $(c_sources)
# 	$(CC) -c -g $(atkcflags) $(gtkcflags)  $(includeflags) $< -o $@ \
# 	$(atklflags) $(gtklflags) $(x11lflags)
# $(c_objs): $(c_sources)
# 	$(CC) -g $(atkcflags) $(gtkcflags)  $(includeflags) $< -o $@ \
# 	$(atklflags) $(gtklflags) $(x11lflags)

print:
	@echo $(objs)

clean:
	rm -f build/*.o build/main *.o