#include <cairo.h>
#include "cairo-base.h"
#include <stdio.h>
#include <X11/keysym.h>

enum {
  WIDTH=320,
  HEIGHT=200
};

cairo_extend_t extend=CAIRO_EXTEND_REPEAT;
cairo_filter_t filter=CAIRO_FILTER_GOOD;

void draw_it(cairo_t *cr,
	     cairo_surface_t *surface) {
  // Size of the text
  cairo_save(cr);
  cairo_set_font_size(cr, 10);
  cairo_text_extents_t extents;
  cairo_text_extents(cr, "Pattern", &extents);
  cairo_restore(cr);

  //printf("%f %f\n", extents.width, extents.height);

  // new surface should be the pattern
  cairo_surface_t *pattern_surface=cairo_surface_create_similar
    (surface, CAIRO_CONTENT_COLOR, extents.width+2, extents.height+2);

  // render text on the surface
  cairo_t *cr2=cairo_create(pattern_surface);
  cairo_set_source_rgb(cr2, 1.0, 1.0, 1.0);
  cairo_rectangle(cr2, 0.0, 0.0, extents.width+2, extents.height+2);
  cairo_fill(cr2);
  cairo_set_font_size(cr2, 10);
  cairo_set_source_rgb(cr2, 0.2, 0.4, 0.5);
  cairo_move_to(cr2, 0.0, 7.0);
  cairo_show_text(cr2, "Pattern");
  cairo_destroy(cr2);

  // create the pattern
  cairo_pattern_t *pattern=cairo_pattern_create_for_surface(pattern_surface);

  cairo_pattern_set_extend(pattern, extend);
  cairo_pattern_set_filter(pattern, filter);

  // fill
  cairo_set_source(cr, pattern);
  cairo_rectangle(cr, 0.0, 0.0, WIDTH, HEIGHT);
  cairo_fill(cr);

  // print text
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

int main() {
  cairo_base *ptr=base_init(WIDTH, HEIGHT, draw_it, NULL);
  base_loop(ptr);
  base_free(ptr);
}
