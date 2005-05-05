bin_PROGRAMS=gui-test
gui_test_SOURCES= gui-test.cpp
gui_test_CXXFLAGS= @GL_CFLAGS@ @SIGC_CFLAGS@ -I$(top_srcdir)/include/
gui_test_LDADD= @GL_LIBS@ @SIGC_LIBS@ $(top_builddir)/source/glsk/libglsk.a $(top_builddir)/source/glskcore/libglskcore.la $(top_builddir)/source/glskmm/libglskmm.la -lboost_signals

