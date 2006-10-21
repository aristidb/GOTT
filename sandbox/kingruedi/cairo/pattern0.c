#include <cairo.h>
#include "cairo-base.h"
#include <stdio.h>

enum {
  WIDTH=320,
  HEIGHT=200
};

cairo_extend_t extend=CAIRO_EXTEND_REPEAT;
cairo_filter_t filter=CAIRO_FILTER_GOOD;

void draw_it(cairo_t *cr,
	     cairo_surface_t *surface) {
  cairo_pattern_t *pattern=cairo_pattern_create_linear(0.0, 0.0,
						       WIDTH, HEIGHT);

  cairo_pattern_set_extend(pattern, extend);
  cairo_pattern_set_filter(pattern, filter);
  cairo_pattern_add_color_stop_rgba(pattern, 0,
				    1.0, 0.0, 0.0, 0.7);
  cairo_pattern_add_color_stop_rgba(pattern, 0.2,
				    0.8, 0.2, 0, 0.5);
  cairo_pattern_add_color_stop_rgba(pattern, 0.4,
				    0.6, 0.2, 0.2, 0.5);
  cairo_pattern_add_color_stop_rgba(pattern, 0.6,
				    0.4, 0.4, 0.2, 0.5);
  cairo_pattern_add_color_stop_rgba(pattern, 0.8,
				    0.2, 0.4, 0.4, 0.3);
  cairo_pattern_add_color_stop_rgba(pattern, 1.0,
				    0.0, 0.6, 0.4, 0.2);
  cairo_set_source(cr, pattern);
  cairo_rectangle(cr, 0.0, 0.0, WIDTH, HEIGHT);
  cairo_fill(cr);

  cairo_save(cr);
  cairo_set_source_rgba(cr, 0.0, 0.2, 0.1, 0.6);
  cairo_move_to(cr, 100, 100);
  cairo_set_font_size(cr, 20);
  cairo_show_text(cr, "Hallo Muster!");
  cairo_restore(cr);

  if(cairo_status(cr)) {
    fprintf(stderr, "Cairo: %s\n",
	    cairo_status_to_string(cairo_status(cr)));
    exit(1);
  }

  cairo_pattern_destroy(pattern);
}

void key(KeySym keycode) {
  switch(keycode) {
  case XK_e:
    if(++extend>CAIRO_EXTEND_REFLECT)
      extend=CAIRO_EXTEND_NONE;
    printf("Extend: %d\n", extend);
    break;
  case XK_f:
    if(++filter>CAIRO_FILTER_GAUSSIAN)
      filter=CAIRO_FILTER_FAST;
    printf("Filter: %d\n", filter);
    break;
  case NoSymbol:
    puts("grml\n");
  }
}

int main() {
  cairo_base *ptr=base_init(WIDTH, HEIGHT, draw_it, key);
  base_loop(ptr);
  base_free(ptr);
}
