#ifndef SAMPLE_VECTOR_HPP
#define SAMPLE_VECTOR_HPP

#include "../include/vector_traits.hpp"
#include <cstddef>

namespace sample {

  template<unsigned int D>
  class vector {
  public:
    vector(const vector& v) {
      memcpy(values,v.values,sizeof(float)*D);
    }

    const vector& operator = (const vector& v) {
      memcpy(values,v.values,sizeof(float)*D);
      return *this;
    }

    float values[D];
  };
}

namespace vino {

  template<unsigned int D>
  struct vector_traits<sample::vector<D> > {
    typedef sample::vector<D> vector_type;
    typedef float value_type;

    static const unsigned int dimension = D;

    static void
    assign(vector_type& v1, const vector_type& v2) {
      v1 = v2;
    }

    static void
    copy(vector_type* v1, const vector_type* v2, size_t n) {
      memcpy(v1,v2,sizeof(vector_type)*n);
    }

    static value_type
    get_x(const vector_type& v) { return v[0]; }
    
    static value_type
    get_y(const vector_type& v) { return v[1]; }

    static value_type
    get_z(const vector_type& v) { return v[2]; }

    static value_type
    get_w(const vector_type& v) { return v[3]; }
  };
}

#endif
