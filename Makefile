atkcflags=`pkg-config --cflags atk`
gtkcflags=`pkg-config --cflags gtk+-2.0`
includeflags=-I. -Ixlib_test -Igtk_test -Ixcb_test


atklflags=`pkg-config --libs atk`
gtklflags=`pkg-config  --libs gtk+-2.0`
gdkpixbuflflags=`pkg-config --libs gdk-pixbuf-xlib-2.0`
x11lflags= -lX11 -lXext -lXt -ljpeg -L. -Lxlib_test $(gdkpixbuflflags) -lGL -lXi

CC = gcc
CXX = g++

cpp_sources= $(wildcard *.cc xlib_test/*.cc xcb_test/*.cc gtk_test/*.cc)
c_sources = $(wildcard *.c xlib_test/*.c xcb_test/*.c gtk_test/*.c)

cpp_objs=$(patsubst %.cc,%.o,$(cpp_sources))
c_objs= $(patsubst %.c,%.o,$(c_sources))

# cpp_objs=$(addprefix build/, $(patsubst %.cc,%.o,$(notdir $(cpp_sources))) )
# c_objs= $(addprefix build/, $(patsubst %.c,%.o,$(notdir $(c_sources))) )

#objs   = $(patsubst %.cc,%.o,$(cpp_sources) )  $(patsubst %.c,%.o,$(c_sources) )

CFLAGS =  -g $(atkcflags) $(gtkcflags) $(includeflags)
CPPFLAGS =  -g $(atkcflags) $(gtkcflags) $(includeflags)

main: $(c_objs) $(cpp_objs)
	$(CXX) -g $(atkcflags) $(gtkcflags) $(includeflags) \
	-o main $(cpp_sources) $(c_sources)  \
	$(atklflags) $(gtklflags) $(x11lflags)


# main: $(c_objs) $(cpp_objs)
# 	$(CXX)  -g -o main $(c_objs) $(cpp_objs) \
#     $(atklflags) $(gtklflags) $(x11lflags)

# $(cpp_objs): $(cpp_sources)
# 	$(CXX) -g -c $(atkcflags) $(gtkcflags) $(includeflags) $< -o $@ $(atklflags) $(gtklflags) $(x11lflags)

# $(c_objs): $(c_sources)
# 	$(CXX) -g -c $(atkcflags) $(gtkcflags) $(includeflags) $< -o $@ $(atklflags) $(gtklflags) $(x11lflags)

print:
	@echo $(objs)

clean:
	rm -f main $(cpp_objs) $(c_objs)
