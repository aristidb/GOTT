INCLUDES=-I$(top_srcdir)/include/
EXTRA_DIST= Makefile.am \
						w_access.c \
						w_extensions.c \
						w_input.c \
						w_keycode_list.gperf \
						w_main.c \
						w_pixelformat.c \
						w_window.c \
						x_access.c \
						x_main.c \
						x_pixelformat.c \
						x_input.c \
						x_pbuffer.c \
						x_input.h \
						w_extensions.h \
						w_input.h \
						w_main.h\
						w_pixelformat.h\
						w_window.h\
						x_access.h\
						x_keycode_list.gperf \
						x_main.h\
						x_pixelformat.h

lib_LTLIBRARIES=libglskcore.la 

if WINDOWS
GPERF_STUFF=w_keycode_list.lo
libglskcore_la_SOURCES= w_access.c \
												w_extensions.c \
												w_input.c \
												w_main.c \
												w_pixelformat.c \
												w_pbuffer.c \
												w_window.c 
endif
if NORMAL
GPERF_STUFF=x_keycode_list.lo
libglskcore_la_SOURCES= x_access.c \
												x_main.c \
												x_pixelformat.c \
												x_input.c \
												x_pbuffer.c
endif

## Darwin implementation is missing! :)


libglskcore_la_CFLAGS=@GL_CFLAGS@ 
libglskcore_la_LIBADD=@GL_LIBS@ $(GPERF_STUFF)
libglskcore_la_LDFLAGS=-version-info $(GLSKCORE_LIBRARY_VERSION) -release $(GLSKCORE_RELEASE)

## w_keycode_list.o: w_keycode_list.c ../glsk/glsk.h
## 	@GCC@  $< -o $@ 

w_keycode_list.c: w_keycode_list.gperf
	@GPERF@ $< --output-file=$@
	
x_keycode_list.c: x_keycode_list.gperf
	@GPERF@ $< --output-file=$@
