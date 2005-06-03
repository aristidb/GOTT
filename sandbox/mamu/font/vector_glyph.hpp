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
    struct mesh_info {  
      GLenum mode; 
      std::size_t count, first;  
      mesh_info( GLenum mode, std::size_t count, std::size_t first ); 
    };
    typedef gott::math::vector3<double> v3_type;
    typedef std::vector<v3_type> mesh_type;
    typedef std::list<mesh_info> info_list;
    
  private:
    mesh_type va, orig_data;
    info_list ranges;

    void conic( std::vector<v3_type>& verts, v3_type const& begin, v3_type const& mid, v3_type const& end ) const;  
    void cubic( std::vector<v3_type>& verts, v3_type const& begin, v3_type const& mid_1, v3_type const& mid_2, v3_type const& end ) const; 

    struct ft_info {
      std::vector<v3_type > verts;
      typedef std::list<std::pair<size_t, size_t> > contour_list; 
      contour_list contours;
    };

    static void move_to( FT_Vector * to, ft_info * p );
    static void line_to( FT_Vector * to, ft_info * p );
    static void conic_to( FT_Vector * control, FT_Vector * to, ft_info * p );
    static void cubic_to( FT_Vector * control1, FT_Vector * control2, FT_Vector * to, ft_info * p );
    
    static void begin( GLenum type, vector_glyph * p );
    static void end( vector_glyph * p );
    static void vertex( v3_type * data, vector_glyph * p );
    static void combine( GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **outData,  vector_glyph * ptr ) ;
    static void error( GLenum type, vector_glyph * p );
  public:
    vector_glyph( FT_Face & face, std::size_t glyph_index );
    void render() const;

};

}}}

#endif 

