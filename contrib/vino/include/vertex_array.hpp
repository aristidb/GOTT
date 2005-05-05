#ifndef VINO_VERTEX_ARRAY_HPP
#define VINO_VERTEX_ARRAY_HPP

#include <utility>
#include <vector>

namespace vino {
  
  template<typename VERTEX, typename TRAITS = vertex_traits<VERTEX> >
  class vertex_array {
  public:

    void render();
    
    void set_pos_pointer(TRAITS::vertex_pos_traits::value_type* p,
                         unsigned int stride = 0) {
      
    }

    void set_color_pointer(TRAITS::vertex_color_traits::value_type* p,
                           unsigned int stride = 0) {
      
    }

    void set_color_

  private:
    vertex_array(unsigned int mtl) : {
      // warn user if TRAITS::num_layers > mtl
      max_texture_layers = TRAITS::num_layers > mtl ? mtl : TRAITS::num_layers;
    }

    unsigned int max_texture_layers;
    
  };
}

#endif
