// Copyright (C) 2005 by Ruediger Sonderfeld <kingruedi@c-plusplus.de>

#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_renderer_primitives.h"
#include "agg_pixfmt_rgb.h"
#include "platform/agg_platform_support.h"

typedef agg::pixfmt_bgr24 pixfmt;
typedef pixfmt::color_type color_type;
typedef agg::renderer_base<pixfmt> base_renderer;
typedef agg::renderer_primitives<base_renderer> primitives_renderer;

class the_application : public agg::platform_support {
public:
  the_application(agg::pix_format_e format, bool flip_y)
    : agg::platform_support(format, flip_y)
  {
  }

  void on_draw() {
    pixfmt pixf(rbuf_window());
    base_renderer rb(pixf);

    rb.clear(agg::rgba8(0x0,0x0,0x0));
    rb.blend_pixel(200,200,agg::rgba8(0x0,0xff,0x0),agg::cover_full);

    primitives_renderer prim(rb);

    prim.line_color(agg::rgba8(0xff,0x0,0x0));
    prim.line(0,0, 400*256,400*256); //1/256 SubPixel Accuracy!

    prim.fill_color(agg::rgba8(0x00,0x0,0xff));
    prim.outlined_rectangle(10, 10, 20, 20);
  }
};

int agg_main(int, char **) {
  the_application app(agg::pix_format_bgr24, true);
  app.caption("AGG test");

  if(app.init(400, 400, agg::window_resize))
    return app.run();

  return 1;
}
