#ifndef SAMPLE_VERTEX_HPP
#define SAMPLE_VERTEX_HPP

#include "../include/vertex_traits.hpp"
#include "sample_vector.hpp"
#include "sample_color.hpp"

namespace sample {
  
  class vertex {
  public:
    vector<3> pos_vec;
    rgba_color color;
    vector<2> tex_vec;
    vector<3> nrm_vec;
  };
}

namespace vino {
  
  template<>
  struct vertex_traits<sample::vertex> {
    typedef sample::vertex vertex_type;
    
    typedef vector_traits<sample::vector<3> > vertex_pos_traits;
    typedef rgba_color_traits<sample::rgba_color> vertex_color_traits;
    static const bool has_color = true;
    typedef vector_traits<sample::vector<2> > vertex_texcoord_traits;
    static const bool has_texcoords = true;
    static const unsigned int num_texcoords = 1;
    typedef vector_traits<sample::vector<3> > vertex_normal_traits;
    static const bool has_normal = true;

    static void
    assign(vertex_type& v1, const vertex_type& v2) {
      vertex_pos_traits::assign(v1.pos_vec, v2.pos_vec);
      vertex_color_traits::assign(v1.color, v2.color);
      vertex_texcoord_traits::assign(v1.tex_vec, v2.tex_vec);
      vertex_normal_traits::assign(v1.nrm_vec, v2.nrm_vec);
    }

    static void
    copy(vertex_type* v1, const vertex_type* v2, size_t n) {
      memcpy(v1,v2,sizeof(vertex_type) * n);
    }

    static const void*
    get_pos_pointer(const vertex_type& v) {
      return &v;
    }

    static const void*
    get_color_pointer(const vertex_type& v) {
      return &(v.color);
    }

    static const void*
    get_texcoord_pointer(const vertex_type& v) {
      return &(v.tex_vec);
    }

    static const void*
    get_normal_pointer(const vertex_type& v) {
      return &(v.nrm_vec);
    }

    static size_t
    get_stride() {
      return sizeof(vertex_type);
    }
  };
}

#endif
