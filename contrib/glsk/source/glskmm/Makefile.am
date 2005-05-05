
EXTRA_DIST= Makefile.am \
						glskmm.cpp 
						
lib_LTLIBRARIES=libglskmm.la

libglskmm_la_SOURCES=glskmm.cpp 

libglskmm_la_CXXFLAGS=@SIGC_CFLAGS@ -I$(top_srcdir)/include/
libglskmm_la_LIBADD=@SIGC_LIBS@
libglskmm_la_LDFLAGS=-version-info $(GLSKMM_LIBRARY_VERSION) -release $(GLSKMM_RELEASE)


