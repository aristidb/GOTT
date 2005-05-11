CXXFLAGS=-O0 -ggdb3


all: application.o pixelformat.o application_x11.o window.o window_x11.o gl_context_x11.o globals_x11.o test_app.o
	$(CXX) $(CXXFLAGS) application.o pixelformat.o application_x11.o window.o window_x11.o gl_context_x11.o globals_x11.o test_app.o  -o $@ -lGL -lGLU -lXext -lX11 -lboost_signals -lXxf86vm 

clean:
	rm -f *.o all


