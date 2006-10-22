#include "../include/vertex_array.hpp"
#include "sample_vertex.hpp"

int main(int, char**) {
  vino::vertex_array<sample::vertex> va(1);

  sample::rgba_color c(0,0,0,0);

  va.set_color_pointer((vino::rgba_color_traits<sample::rgba_color>::value_type *)(&c), 0);

  return 0;
}
