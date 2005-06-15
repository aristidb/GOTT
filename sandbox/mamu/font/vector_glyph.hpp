#ifndef GOTT_GUI_FONT_VECTOR_GLYPH_HPP_INCLUDED
#define GOTT_GUI_FONT_VECTOR_GLYPH_HPP_INCLUDED

#include <vector>
#include <list>
#include <GL/gl.h>
#include <ft2build.h>
#include  FT_FREETYPE_H 
#include "../math/fixed_vector.hpp"
#include "../math/clockwise.hpp"

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

  private:
    std::vector<v2_type> vertex_array;
    std::vector<mesh_info> array_info;

    static void begin_array(GLenum mode, vector_glyph *glyph);
    static void end_array(vector_glyph *glyph );
    static void combine(  GLdouble coords[3], void *vertex_data[4],
                                    GLfloat weight[4], void **outData,
                                    vector_glyph *glyph);
    static void add_vertex( gott::math::vector3<double>* vertex_data, vector_glyph * glyph );

    void conic( GLUtesselator *t, std::vector<gott::math::vector3<double>*> & data, v2_type const& begin, v2_type const& mid, v2_type const& end ) const;  
    void cubic( GLUtesselator *t, std::vector<gott::math::vector3<double>*> & data, v2_type const& begin, v2_type const& mid_1, v2_type const& mid_2, v2_type const& end ) const;  

  public:
    vector_glyph( FT_Face & face, std::size_t glyph_index );
    void render() const;

};

}}}

#endif 

