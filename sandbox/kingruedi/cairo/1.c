#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

enum { WIDTH=300,
       HEIGHT=200 };

void cairo_draw(Display *display, Drawable window, int screen) {
  XClearWindow(display, window);

  cairo_surface_t *surface = cairo_xlib_surface_create
    (display, window, DefaultVisual(display, screen), WIDTH, HEIGHT);
  cairo_t *cr=cairo_create(surface);

  cairo_set_source_rgba(cr, 0.2, 0.2, 0, 0.6);
  cairo_set_line_width(cr, 0.5);

  cairo_new_path(cr);
  cairo_move_to(cr, 0.1, 0.1);
  cairo_rel_line_to(cr, 10.8, 10);
  cairo_rel_line_to(cr, 0, 0.8);
  cairo_close_path(cr);

  if(cairo_status(cr)) {
    fprintf(stderr, "Cairo: %s\n",
	   cairo_status_to_string(cairo_status(cr)));
    exit(1);
  }

  cairo_destroy(cr);
  cairo_surface_destroy (surface);
}

int main() {
  Display *display=XOpenDisplay(NULL);
  int screen=DefaultScreen(display);
  XSetWindowAttributes attribute;
  attribute.background_pixel=WhitePixel(display,screen);
  Window window=XCreateWindow(display, RootWindow(display,screen), 0, 0, WIDTH,
			      HEIGHT, 1, DefaultDepth(display,screen),
			      InputOutput, DefaultVisual(display,screen),
			      CWBackPixel, &attribute);

  XSelectInput(display, window, ExposureMask);

  XMapWindow(display,window);
  XFlush(display);

  cairo_draw(display, window, screen);

  for(;;)
  {
    XEvent event;
    XNextEvent(display,&event);
    switch(event.type)
    {
    case Expose:
      if(event.xexpose.count>0)
        break;
      cairo_draw(display, window, screen);
      break;
    }
  }

  XCloseDisplay(display);
}

