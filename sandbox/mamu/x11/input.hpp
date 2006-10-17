#include <X11/Xdefs.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include "../input.hpp"

namespace gott{ namespace gui{ namespace x11{
  
struct key_table
{
  private:
    unsigned char t[2][256];
    key_table();
    key_table(key_table const&);
    key_table& operator=(key_table const&);
  public:
    key_code translate_key( KeySym const& sym ) const;
    static key_table const & get_instance();
};

}}}

