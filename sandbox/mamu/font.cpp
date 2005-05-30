
#include "font.hpp"
namespace gott{namespace gui{namespace font{

namespace{
struct ft_manager{
  FT_Library library;
  ft_manager() {
    FT_Init_FreeType( &library );
  }
  ~ft_manager() {
  }
};
}
FT_Library& get_instance() { static ft_manager instance; return instance.library;}
}}}
