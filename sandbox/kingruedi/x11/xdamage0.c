#include <stdio.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>

enum { WIDTH=200,
       HEIGHT=200 };

#define MAX(a, b) ((a)>(b) ? (a) : (b))
#define MIN(a, b) ((a)>(b) ? (b) : (a))

bool rect_intersect(XRectangle const *a, XRectangle const *b) {
  short a_w=a->x + a->width;
  short a_h=a->y + a->height;
  short b_w=b->x + b->width;
  short b_h=b->y + b->height;

  if(MIN(a_w, b_w) - MAX(a->x, b->x) > 0 &&
     MIN(a_h, b_h) - MAX(a->y, b->y) > 0)
    return true;
  return false;
}

int main()
{
  Display *display = XOpenDisplay(NULL);
  int screen = DefaultScreen(display);
  Visual *visual = DefaultVisual(display,screen);
  int depth  = DefaultDepth(display,screen);

  XSetWindowAttributes attributes;
  attributes.background_pixel = XWhitePixel(display,screen);
 
  Window window = XCreateWindow(display, XRootWindow(display,screen),
				0, 0, WIDTH, HEIGHT, 1, depth,  InputOutput,
				visual ,CWBackPixel, &attributes);
  XSelectInput(display, window, ExposureMask | KeyPressMask | XDamageNotify);

  int xdamage_event_base, xdamage_error_base;
  if(!XDamageQueryExtension(display, &xdamage_event_base, &xdamage_error_base))
    return 1;
  Damage damage=XDamageCreate(display, window, XDamageReportRawRectangles);

  XColor fcolor, bcolor, dummy;
  XAllocNamedColor(display, DefaultColormap(display, screen), "red",
		   &fcolor, &dummy);
  XAllocNamedColor(display, DefaultColormap(display, screen), "blue",
		   &bcolor, &dummy);

  XGCValues gc_values;
  gc_values.foreground = fcolor.pixel;
  gc_values.background = bcolor.pixel;
  GC gc=XCreateGC(display, window, GCForeground|GCBackground, &gc_values);
  XFlush(display);
  XMapWindow(display, window);
  XFlush(display);

  enum { rects_n=(WIDTH*HEIGHT)/20 };
  XRectangle rects[rects_n];

  size_t n=0;
  for(unsigned int x=0; x<WIDTH; x+=20) {
    for(unsigned int y=0; y<HEIGHT; y+=20) {
      rects[n].x=x+1;
      rects[n].y=y+1;
      rects[n].width=rects[n].height=18;
      XDrawRectangle(display, window, gc, rects[n].x, rects[n].y,
		     rects[n].width, rects[n].height);
      //XserverRegion damage_r=XFixesCreateRegion(display, &rects[n], 1);
      //XDamageSubtract(display, damage, None, damage_r);
      ++n;
    }
  }

  while(1){
    XEvent event;
    XNextEvent(display, &event);

    if(event.type == KeyPress) {
      XDamageDestroy(display, damage);
      XCloseDisplay(display); 
      return 0;
    }
    else if(event.type == xdamage_event_base+XDamageNotify) {
      XDamageNotifyEvent *notify=(XDamageNotifyEvent *)&event;
      printf("Damage: (%d,%d)(%d,%d)\n", notify->area.x, notify->area.y,
	     notify->area.width, notify->area.height);

      for(size_t n; n<rects_n; ++n) {
	if(rect_intersect(&rects[n], &notify->area))
	  XDrawRectangle(display, window, gc, rects[n].x, rects[n].y,
			 rects[n].width, rects[n].height);
      }
      XFlush(display);
    }
  }
  return 1;
}

/*

See
/usr/include/X11/extensions/Xdamage.h
http://cvs.freedesktop.org/xlibs/DamageExt/protocol?rev=HEAD&view=markup
http://mail.gnome.org/archives/desktop-devel-list/2004-May/msg00289.html

 */
