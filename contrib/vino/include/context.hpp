#ifndef VINO_CONTEXT_HPP
#define VINO_CONTEXT_HPP

#include <boost/type_traits.hpp>
#include <boost/mpl/int.hpp>
#include <boost/enable_if.hpp>
#include "error.hpp"
#include "vector_traits.hpp"
#include "color_traits.hpp"
#include "texture_layer.hpp"

namespace vino {

  class context {
  public:
    // == CON/DESTRUCTOR STUFF ==
    context() {
      glGetParameteriv(GL_MAX_TEXTURE_UNITS, &max_texture_units);
    }
    
    // == MISC ==
    // texture layer
    texture_layer tl(unsigned int n) throw(vino_error) {
      if(n >= max_texture_units)
        throw error(std::string("\
there aren't that many texturelayer supportet by this opengl implementation"));
      return texture_layer(n);
    }

    // == DRAWING ==
    // begin/end
    enum drawing_object_type {
      dot_points = GL_POINTS,
      dot_line_strip = GL_LINE_STRIP,
      dot_line_loop = GL_LINE_LOOP,
      dot_lines = GL_LINES,
      dot_polygon = GL_POLYGON,
      dot_triangle_strip = GL_TRIANGLE_STRIP,
      dot_triangle_fan = GL_TRIANGLE_FAN,
      dot_triangles = GL_TRIANGLES,
      dot_quad_strip = GL_QUAD_STRIP,
      dot_quads = GL_QUADS
    };
    void begin(drawing_object_type dot) {
      glBegin(GLenum(dot));
    }
    void end() {
      glEnd();
    }

    // vertex
    void vertex(float x, float y, float z, float w) {
      glVertex4f(x,y,z,w);
    }

    void vertex(float x, float y, float z) {
      glVertex3f(x,y,z);
    }

    void vertex(float x, float y) {
      glVertex2f(x,y);
    }

    template<typename V>
    void vertex(const V& v,
                boost::enable_if<
                vector_traits<V>::dimension >= 4
                >::type *P = 0) {
      glVertex4f(vector_traits<V>::get_x(v),
                 vector_traits<V>::get_y(v),
                 vector_traits<V>::get_z(v),
                 vector_traits<V>::get_w(v));
    }

    template<typename V>
    void vertex(const V& v,
                boost::enable_if<
                vector_traits<V>::dimension == 3
                >::type *P = 0) {
      glVertex3f(vector_traits<V>::get_x(v),
                 vector_traits<V>::get_y(v),
                 vector_traits<V>::get_z(v));
    }

    template<typename V>
    void vertex(const V& v,
                boost::enable_if<
                vector_traits<V>::dimension == 2
                >::type *P = 0) {
      glVertex2f(vector_traits<V>::get_x(v),
                 vector_traits<V>::get_y(v));
    }

    // texcoord
    void texcoord(float s,
                  texture_layer tl = texture_layer(0)) {
      glMultiTexCoord1f(tl.layer_number,x);
    }
    
    void texcoord(float s, float t,
                  texture_layer tl = texture_layer(0)) {
      glMultiTexCoord2f(tl.layer_number, s, t);
    }

    void texcoord(float s, float t, float r,
                  texture_layer tl = texture_layer(0)) {
      glMultiTexCoord3f(tl.layer_number, s, t, r);
    }

    void texcoord(float s, float t, float r, float q,
                  texture_layer tl = texture_layer(0)) {
      glMultiTexCoord4f(tl.layer_number, s, t, r, q);
    }

    template<typename V>
    void texcoord(const V& v,
                  texture_layer tl = texture_layer(0)
                  boost::enable_if<
                  vector_traits<V>::dimension == 1
                  >::type *P = 0) {
      glMultiTexCoord1f(tl.layer_number,
                        vector_traits<V>::get_x(v));
    }
    
    template<typename V>
    void texcoord(const V& v,
                  texture_layer tl = texture_layer(0)
                  boost::enable_if<
                  vector_traits<V>::dimension == 2
                  >::type *P = 0) {
      glMultiTexCoord2f(tl.layer_number,
                        vector_traits<V>::get_x(v),
                        vector_traits<V>::get_y(v));
    }


    template<typename V>
    void texcoord(const V& v,
                  texture_layer tl = texture_layer(0)
                  boost::enable_if<
                  vector_traits<V>::dimension == 3
                  >::type *P = 0) {
      glMultiTexCoord3f(tl.layer_number,
                        vector_traits<V>::get_x(v),
                        vector_traits<V>::get_y(v),
                        vector_traits<V>::get_z(v));
    }


    template<typename V>
    void texcoord(const V& v,
                  texture_layer tl = texture_layer(0)
                  boost::enable_if<
                  vector_traits<V>::dimension >= 4
                  >::type *P = 0) {
      glMultiTexCoord3f(tl.layer_number,
                        vector_traits<V>::get_x(v),
                        vector_traits<V>::get_y(v),
                        vector_traits<V>::get_z(v),
                        vector_traits<V>::get_w(v));
    }

    // normal
    void normal(float x, float y, float z) {
      glNormal3f(x,y,z);
    }

    template<typename V>
    void normal(const V& v,
                boost::enable_if<
                vector_traits<V>::dimension >= 3
                >::type *P = 0) {
      glNormal3f(vector_traits<V>::get_x(v),
                 vector_traits<V>::get_y(v),
                 vector_traits<V>::get_z(v));
    }

    // fogcoord
    void fogcoord(float z) {
      glFogCoordf(z);
    }

    template<typename V>
    void fogcoord(const V& v,
                  boost::enable_if<
                  vector_traits<V>::dimension >= 1
                  >::type *P = 0) {
      glFogCoordf(vector_tratis<V>::get_x(v));
    }

    // Color
    void color(float r, float g, float b) {
      glColor3f(r, g, b);
    }

    void color(float r, float g, float b, float a) {
      glColor4f(r, g, b, a);
    }

    template<typename V>
    void color(const V& v,
               boost::enable_if<boost::is_same<
               rgba_color_traits<V>::coord_type,float
               > >::type *P = 0) {
      glColor4f(rgba_color_traits<V>::red(v),
                rgba_color_traits<V>::green(v),
                rgba_color_traits<V>::blue(v),
                rgba_color_traits<V>::alpha(v));
                
    }

    void color(char r, char g, char b) {
      glColor3b(r, g, b);
    }

    void color(char r, char g, char b, char a) {
      glColor4b(r, g, b, a);
    }

    void fogcoord(const V& v,
                  boost::enable_if<boost::is_same<
                  rgba_color_traits<V>::coord_type,char
                  > >::type *P = 0) {
      glColor4b(rgba_color_traits<V>::red(v),
                rgba_color_traits<V>::green(v),
                rgba_color_traits<V>::blue(v),
                rgba_color_traits<V>::alpha(v));
    }

    // == STATE CHANGEING FUNCTIONS ==
    // == callable between begin and end ==
    
    // edgeflag
    void edgeflag(bool flag) {
      glEdgeFlag(flag);
    }
    
    // == not callable between begin and end ==
  private:
    const unsigned int max_texture_units;
  };
}

#endif
