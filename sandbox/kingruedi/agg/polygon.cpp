// Copyright (C) 2005 by Ruediger Sonderfeld <kingruedi@c-plusplus.de>

// Basicly copied from the AGG examples

#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"
#include "agg_trans_affine.h"
#include "agg_conv_stroke.h"
#include "agg_conv_transform.h"
#include "agg_pixfmt_rgb.h"
#include "agg_path_storage.h"
#include "platform/agg_platform_support.h"

#include <iostream>

struct path_attributes {
    unsigned   index;
    agg::rgba8 fill_color;
    agg::rgba8 stroke_color;
    double     stroke_width;

    path_attributes() {}
    path_attributes(unsigned idx, 
                    agg::rgba8 const& fill,
                    agg::rgba8 const& stroke,
                    double width)
      : index(idx),
	fill_color(fill),
	stroke_color(stroke),
	stroke_width(width)
    {
    }
};

#define AGG_POLY_SIZE(p) (sizeof(p) / (sizeof(*p) * 2))

inline double to_radian(double a) {
  return a*agg::pi/180;
}

class the_application : public agg::platform_support {
  agg::path_storage path;
  path_attributes attr;

  agg::scanline_p8              scanline;
  agg::rasterizer_scanline_aa<> rasterizer;

  double angle;
  bool redraw_flag;
public:
  the_application(agg::pix_format_e format, bool flip_y)
    : agg::platform_support(format, flip_y),
      angle(12),
      redraw_flag(true)
  {
    double const vertices0[]={
      /*0,   0,
      0,   100,
      100, 100,
      100, 0*/

      /*0, 100,
      75, 125,
      100, 200,
      125, 125,
      200, 100,
      125, 75,
      100, 0,
      75, 75,
      0, 100*/

      /*0, 2,
      1.5, 2.5,
      2, 4,
      2.5, 2.5,
      4, 2,
      2.5, 1.5,
      2, 0,
      1.5, 1.5*/

      0, 4,
      3, 5,
      4, 8,
      5, 5,
      8, 4,
      5, 3,
      4, 0,
      3, 3,
      0, 4
    };

    // Create a new path (Polygon)
    attr = path_attributes(path.start_new_path(),
			   agg::rgba8(0xfa,0xf7,0x20),
			   agg::rgba8(0,0,0),
			   0.5);

    path.add_poly(vertices0, AGG_POLY_SIZE(vertices0));

    /*
      path.start_new_path returns an index to the current Vertex.
      You can simply add all Vertices to path and remember the beginning of
      each figure.
     */

    wait_mode(false);
  }

  void resize(int,int) {
    redraw_flag=true;
  }

  void on_draw() {
    typedef agg::pixfmt_bgr24 pixfmt;
    typedef agg::renderer_base<pixfmt> renderer_base;
    typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;

    pixfmt pixf(rbuf_window());
    renderer_base rbase(pixf);
    renderer_solid rsolid(rbase);

    if(redraw_flag) {
      rbase.clear(agg::rgba8(0xff,0xff,0xff));
      redraw_flag=false;
    }
    else
      rbase.copy_bar(0, 0,
		     rbuf_window().width(), rbuf_window().height(),
		     agg::rgba8(0xff,0xff,0xff));

    agg::trans_affine mtx;
    mtx.reset();
    mtx *= agg::trans_affine_rotation(to_radian(angle));
    mtx *= agg::trans_affine_translation(100, 100);
    //    mtx *= agg::trans_affine_scaling(100);

    agg::conv_transform<agg::path_storage> fill(path, mtx);
    agg::conv_stroke<agg::conv_transform<agg::path_storage> > stroke(fill);

    //Render fill
    rasterizer.filling_rule(agg::fill_non_zero);
    rsolid.color(attr.fill_color);
    rasterizer.add_path(fill, attr.index);

    agg::render_scanlines(rasterizer, scanline, rsolid);

    //Render stroke
    if(attr.stroke_width > 0.001) {
      rsolid.color(attr.stroke_color);
      stroke.width(attr.stroke_width * mtx.scale());
      rasterizer.add_path(stroke, attr.index);

      agg::render_scanlines(rasterizer, scanline, rsolid);
    }
  }

  void on_idle() {
    angle += 1.0;
    if(angle > 360.0)
      angle -= 360.0;
    force_redraw();
  }
};

int agg_main(int, char **) {
  the_application app(agg::pix_format_bgr24, false);
  app.caption("AGG Polygon");

  if(app.init(400, 400, agg::window_resize))
    return app.run();

  return 1;
}
