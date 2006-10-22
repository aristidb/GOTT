
#ifndef GOTT_GUI_FONT_HPP_INCLUDED
#define GOTT_GUI_FONT_HPP_INCLUDED

#include <ft2build.h>
#include  FT_FREETYPE_H 

namespace gott{namespace gui{namespace font{

FT_Library&  get_instance();

}}}
#endif

