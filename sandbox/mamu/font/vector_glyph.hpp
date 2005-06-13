#ifndef GOTT_GUI_FONT_VECTOR_GLYPH_HPP_INCLUDED
#define GOTT_GUI_FONT_VECTOR_GLYPH_HPP_INCLUDED

#include <vector>
#include <list>
#include <GL/gl.h>
#include <ft2build.h>
#include  FT_FREETYPE_H 
#include "../math/fixed_vector.hpp"

namespace gott{namespace gui{namespace font {

struct vector_glyph 
{
  public:
    typedef gott::math::vector2<float> v2_type;
   
    struct mesh_info {
      GLenum mode; 
      size_t first;
      size_t count;
      mesh_info( GLenum e, std::size_t f, std::size_t c ) : mode(e), first(f), count(c) {}
    };
    struct contour_point 
    {
      explicit contour_point( v2_type const& pos,  contour_point * p = 0, contour_point * n = 0 );
      contour_point( float x, float y,  contour_point * p = 0, contour_point * n = 0 );
      v2_type point;
      contour_point *previous;
      contour_point *next;
      int array_index;
    };
  private:

    struct scanline_ordering 
    {
      bool operator()(contour_point const& a, contour_point const& b )
      { return a.point[0] < b.point[0];}
      bool operator()(contour_point const* a, contour_point const* b )
      { return a->point[0] < b->point[0];}
    };
    std::list<contour_point*> points;
    std::list<contour_point*> contours;
    std::vector<v2_type> vertex_array;
    //std::vector<mesh_info> vertex_array;
  //  std::vector<unsigned short> index_array;


    void conic( std::list<contour_point*>& points, contour_point * begin, v2_type const& mid, contour_point * end ) const;  
    void cubic( std::list<contour_point*>& points, contour_point * begin, v2_type const& mid_1, v2_type const& mid_2, contour_point * end ) const; 

  public:
    vector_glyph( FT_Face & face, std::size_t glyph_index );
    void render() const;

};

}}}

#endif 

