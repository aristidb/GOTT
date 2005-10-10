#ifndef CAIRO_BASE_H
#define CAIRO_BASE_H

//#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdlib.h>

typedef void (*draw_func_t)(cairo_t*, cairo_surface_t*);
typedef void (*key_func_t)(KeySym keycode);

typedef struct {
  Display *display;
  Window window;
  int screen;
  cairo_surface_t *surface;
  cairo_t *cr;
  draw_func_t draw;
  key_func_t key;
} cairo_base;

cairo_base *base_init(unsigned long width, unsigned long height,
		      draw_func_t draw, key_func_t key) {
  cairo_base *ret=malloc(sizeof(cairo_base));
  if(!ret)
    return NULL;

  ret->display=XOpenDisplay(NULL);
  ret->screen=DefaultScreen(ret->display);
  XSetWindowAttributes attribute;
  attribute.background_pixel=WhitePixel(ret->display, ret->screen);
  ret->window=XCreateWindow(ret->display,
			    RootWindow(ret->display, ret->screen),
			    0, 0, width, height, 1,
			    DefaultDepth(ret->display, ret->screen),
			    InputOutput, DefaultVisual(ret->display,
						       ret->screen),
			    CWBackPixel, &attribute);

  long event_mask=ExposureMask;
  if(key)
    event_mask|=KeyPressMask;
  XSelectInput(ret->display, ret->window, event_mask);

  ret->surface = cairo_xlib_surface_create
    (ret->display, ret->window, DefaultVisual(ret->display, ret->screen),
     width, height);
  ret->cr=cairo_create(ret->surface);

  ret->draw=draw;
  ret->key=key;
  return ret;
}

void base_loop(cairo_base *ptr) {
  XMapWindow(ptr->display, ptr->window);
  XFlush(ptr->display);

  ptr->draw(ptr->cr, ptr->surface);

  XEvent event;
  for(;;)
  {
    XNextEvent(ptr->display, &event);
    switch(event.type)
    {
    case Expose:
      if(event.xexpose.count == 0)
	ptr->draw(ptr->cr, ptr->surface);
      break;
    case KeyPress:
      if(ptr->key)
	ptr->key(XLookupKeysym(&event.xkey, 0));
      ptr->draw(ptr->cr, ptr->surface);
      break;
    }
  }
}

void base_free(cairo_base *ptr) {
  cairo_destroy(ptr->cr);
  cairo_surface_destroy(ptr->surface);
  XDestroyWindow(ptr->display, ptr->window);
  XCloseDisplay(ptr->display);
}

#endif
