#ifndef VINO_TEXTURE_LAYER_HPP
#define VINO_TEXTURE_LAYER_HPP

namespace vino {

  class context;

  class texture_layer {
    friend class context;
  public:
    texture_layer() : layer_number(0) {}
    texture_layer(const texture_layer & tl)
      : layer_number(tl.layer_number) {}
    const texture_layer & operator = (const texture_layer & tl) {
      layer_number = tl.layer_number;
      return *this;
    }

    unsigned int operator () () const {
      return layer_number;
    }
  private:
    texture_layer(unsigned int n)
      : layer_number(n) {}
    unsigned int layer_number;
  };
  
}

#endif
