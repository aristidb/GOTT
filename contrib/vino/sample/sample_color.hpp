#ifndef SAMPLE_COLOR_HPP
#define SAMPLE_COLOR_HPP

#include "../include/color_traits.hpp"
#include <string.h>

namespace sample {
  
  class rgba_color {
  public:
    rgba_color(unsigned char r,
               unsigned char g,
               unsigned char b,
               unsigned char a) {
      values[0] = r;
      values[1] = g;
      values[2] = b;
      values[3] = a;
    }
    rgba_color(const rgba_color& c) {
      memcpy(values,c.values,sizeof(unsigned char) * 4);
    }

    const rgba_color& operator = (const rgba_color& c) {
      memcpy(values,c.values,sizeof(unsigned char) * 4);
      return *this;
    }
    
    unsigned char values[4];
  private:
    rgba_color();
  };
}

namespace vino {
  
  template<>
  struct rgba_color_traits<sample::rgba_color> {

    typedef sample::rgba_color color_type;
    typedef unsigned char value_type;

    static const unsigned int dimension = 4;
    
    static void
    assign(color_type& c1, const color_type& c2) {
      c1 = c2;
    }

    static void
    copy(color_type* c1, const color_type* c2, size_t n) {
      memcpy(c1, c2, sizeof(color_type) * n);
    }

    static value_type
    red(const color_type& c) {
      return c.values[0];
    }

    static value_type
    green(const color_type& c) {
      return c.values[1];
    }

    static value_type
    blue(const color_type& c) {
      return c.values[2];
    }

    static value_type
    alpha(const color_type& c) {
      return c.values[3];
    }
  };
}

#endif
