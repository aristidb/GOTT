#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>

enum { WIDTH=200,
       HEIGHT=200 };

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

  while(1){
    XEvent event;
    XNextEvent(display, &event);

    switch(event.type){
    case Expose:
      if(event.xexpose.count > 0)
	break;
      for(unsigned int x=0; x<WIDTH; x+=20) {
	for(unsigned int y=0; y<HEIGHT; y+=20) {
	  XRectangle rect;
	  rect.x=x+1;
	  rect.y=y+1;
	  rect.width=rect.height=18;
	  XDrawRectangle(display, window, gc, rect.x, rect.y,
			 rect.width, rect.height);
	  XserverRegion damage_r=XFixesCreateRegion(display, &rect, 1);
	  XDamageSubtract(display, damage, None, damage_r);
	}
      }
      break;
    case KeyPress: 
      XDamageDestroy(display, damage);
      XCloseDisplay(display); 
      return 0;
    }
    if(event.type == xdamage_event_base+XDamageNotify) {
      XDamageNotifyEvent *notify=(XDamageNotifyEvent *)&event;
      printf("Damage: (%d,%d)(%d,%d)\n",notify->area.x, notify->area.y,
	     notify->area.width, notify->area.height);
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
