#ifndef GOTT_GUI_FONT_PIXEL_GLYHP_HPP_INCLUDED
#define GOTT_GUI_FONT_PIXEL_GLYHP_HPP_INCLUDED

#include "../device.hpp"

namespace gott{namespace gui{namespace font {

struct pixel_glyph 
{
  private:
    unsigned int list;
    pixel_glyph( pixel_glyph const& );
    pixel_glyph& operator=( pixel_glyph const& );
  public:
    ~pixel_glyph( );
    pixel_glyph( FT_Face & face, std::size_t glyph_index, device_property const& p );
    void render() const;
};

}}}

#endif

