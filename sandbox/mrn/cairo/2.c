#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

enum { WIDTH=300,
       HEIGHT=200 };
       /*
       Mit großer Macht kommt große Verantwortung!
       
struct Path {
   std::list<std::pair<point2,bool> > // so vielleicht?
   Warum? 
   Dachte es gibt mehr als lineto und moveto curve usw..
   mit mehr als zwei params...  
   oH stimmt
    Es müssen ja auch die einzelnen Pfadsegmenttypen gespeichert werden...
   cairot_t 
   achdem 
   wollen wir das mit nem scenegraph wrappen?
   Vielleicht vorerst nicht
   typedef boost::function<void(cairo_surface_t)> path_object; 
   path_object f = path(<ref?>),move_to(16,16),line_t(WIDTH-16,16),line_to(vector_point2_obj) .... <--?
   Wir könnten so kleine funktoren bauen die das alles forwarden.. 
   dann hätten wir in der boost::function nur eine virtuelle Funktion und könnten das 
   Objekt transportieren.. lazy_cairo++
};*/

void draw(Display *display, Drawable window, int screen) {
  XClearWindow(display, window);

  cairo_surface_t *surface = cairo_xlib_surface_create
    (display, window, DefaultVisual(display, screen), WIDTH, HEIGHT);
  cairo_t *cr=cairo_create(surface);

  cairo_pattern_t *gradient 
    = cairo_pattern_create_linear(0, 0, WIDTH, HEIGHT);
  cairo_pattern_add_color_stop_rgba(gradient, 0.0, 0.4, 0.0, 0.4, 0.2);
  cairo_pattern_add_color_stop_rgba(gradient, 0.5, 1.0, 0.0, 0.0, 0.4);
  cairo_pattern_add_color_stop_rgba(gradient, 1.0, 0.0, 0.4, 0.0, 0.6);
  cairo_set_source(cr, gradient);
  cairo_pattern_destroy(gradient);

  cairo_new_path(cr);
  cairo_move_to(cr, 16, 16);
  cairo_line_to(cr, WIDTH - 16, 16);
  cairo_line_to(cr, WIDTH - 16, HEIGHT - 16);
  cairo_line_to(cr, 16, HEIGHT - 16);
  cairo_close_path(cr);

  cairo_fill(cr);

  cairo_set_source_rgba(cr, 0.2, 0.2, 0.0, 0.6);
  cairo_set_line_width(cr, 1.0);

  cairo_new_path(cr);
  cairo_move_to(cr, 10.5, 10.5);
  cairo_rel_line_to(cr, 100, 0);
  cairo_rel_line_to(cr, 0, 100);
  cairo_curve_to(cr, 80, 80, 120, 10, 280, 180);
  cairo_rel_move_to(cr, -3, 0);
  cairo_set_font_size(cr, 40);
  cairo_text_path(cr, "$");
  cairo_close_path(cr);
  cairo_stroke(cr);

  cairo_set_source_rgba(cr, 0.0, 1.0, 0.0, 0.4);
  cairo_move_to(cr, 100, 100);
  cairo_set_font_size (cr, 20);
  cairo_show_text(cr, "Hallo, Welt du schönste aller!");

  if(cairo_status(cr)) {
    fprintf(stderr, "Cairo: %s\n",
	   cairo_status_to_string(cairo_status(cr)));
    exit(1);
  }

  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

int main() {
  printf("Hello World\n");
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

  draw(display, window, screen);

  for(;;)
  {
    XEvent event;
    XNextEvent(display,&event);
    switch(event.type)
    {
    case Expose:
      if(event.xexpose.count > 0)
        break;

      // flickert. (und zwar wegen erase, unter windows wusst ich mal, wie man
      // das loswird)
      draw(display, window, screen);
      break;
    }
  }

  XCloseDisplay(display);
}
