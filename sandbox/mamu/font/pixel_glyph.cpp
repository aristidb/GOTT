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

  list = glGenLists( 1 );
  glPixelStorei( GL_UNPACK_SWAP_BYTES, false );
  glPixelStorei( GL_UNPACK_LSB_FIRST, false );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glPixelStorei( GL_UNPACK_IMAGE_HEIGHT , 0 );
  glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
  glPixelStorei( GL_UNPACK_SKIP_ROWS , 0 );
  glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );

  if( d.dot_layout == device_property::CRTDiffuse ) {
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
    glNewList(list, GL_COMPILE);
    glDrawPixels(  width, height,  GL_LUMINANCE_ALPHA , GL_UNSIGNED_BYTE, buffer );
    glEndList();
    delete[] buffer;
  }else {
    size_t width = bitmap.width / 3;
    size_t height = bitmap.rows;
    //std::cout << "height : " << height << " " << width*3 << " oder " << width/3 << std::endl;
    unsigned char *buffer = new unsigned char[ height*width * 4];
    std::memset( buffer, 0 , width*height*4 );
    for( std::size_t i = 0; i < height; ++i ) {
      for(  std::size_t j = 0; j < width; ++j ) {
        if( bitmap.buffer[(i*width+j)*3] + bitmap.buffer[(i*width+j)*3 + 1] + bitmap.buffer[(i*width+j)*3 + 2] != 0 ) {
        buffer[((height-1-i)*width + j)*4] = bitmap.buffer[(i*width+j)*3];
        buffer[((height-1-i)*width + j)*4 + 1 ] = bitmap.buffer[(i*width+j)*3 + 1];
        buffer[((height-1-i)*width + j)*4 + 2 ] = bitmap.buffer[(i*width+j)*3 + 2];
        buffer[((height-1-i)*width + j)*4 + 3 ] = 255; 
//          std::max(bitmap.buffer[(i*width+j)*3],std::max(bitmap.buffer[(i*width+j)*3 + 1], bitmap.buffer[(i*width+j)*3 + 2] ) );
        }
      }
    }

    glNewList(list, GL_COMPILE);
    glDrawPixels(  width, height,  GL_RGBA, GL_UNSIGNED_BYTE, buffer );
    delete[] buffer;
    glEndList();
  }
}

pixel_glyph::~pixel_glyph(){ glDeleteLists( list, 1 ); }

void pixel_glyph::render() const{
  glCallList(list);
}

}}}
