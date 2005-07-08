#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <GL/glu.h>
#include <freetype/ftoutln.h>
#include "pixel_glyph.hpp"

using namespace std;

namespace gott{namespace gui{namespace font{


pixel_glyph::pixel_glyph( FT_Face & face, std::size_t glyph_index, device_property const& d ) 
{

  int error;
#if 1
  switch( d.dot_layout ) {
    case device_property::LCDVertical: 
      error =FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_TARGET_LCD_V );break;
    case device_property::LCDHorizontal: 
      error=FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_TARGET_LCD );break;
    case device_property::CRTDiffuse: 
    default: 
      error=FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER );break;
  };
#else
  error=FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER );
#endif
  if(error)
    throw runtime_error("Fehler bei FT_Load_Glyp");

  FT_Bitmap & bitmap = face->glyph->bitmap;

  size_t width = bitmap.width;
  size_t height = bitmap.rows;
  unsigned char *buffer = new unsigned char[ ((height - face->glyph->bitmap_top) + height )* width * 2];
  std::memset( buffer, 0 , width*height*2 );
  for( std::size_t i = 0; i < height; ++i ) {
    for(  std::size_t j = 0; j < width; ++j ) {
      if(  bitmap.buffer[i*width+ j] ) {
        buffer[((height-1-(i + (height - face->glyph->bitmap_top)))*width + j)*2 ] = 255;
        buffer[((height-1-(i + (height - face->glyph->bitmap_top)))*width + j)*2 + 1 ] = bitmap.buffer[i*width+ j];
      }
    }
  }
  list = glGenLists( 1 );
  glPixelStorei( GL_UNPACK_SWAP_BYTES, false );
  glPixelStorei( GL_UNPACK_LSB_FIRST, false );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glPixelStorei( GL_UNPACK_IMAGE_HEIGHT , 0 );
  glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
  glPixelStorei( GL_UNPACK_SKIP_ROWS , 0 );
  glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
  glNewList(list, GL_COMPILE);
  glDrawPixels(  width, height,  GL_LUMINANCE_ALPHA , GL_UNSIGNED_BYTE, buffer );
  glEndList();
}

pixel_glyph::~pixel_glyph(){ glDeleteLists( list, 1 ); }

void pixel_glyph::render() const{
  glCallList(list);
}

}}}
