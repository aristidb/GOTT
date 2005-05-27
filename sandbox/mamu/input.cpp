#include "input.hpp"
#include <cstring>

namespace gott{namespace gui{

key_event::key_event( key_code c, event_type t)  
    : type(t), code(c)
{}

key_state::key_state()
  : keyboard( std::size_t(gott::gui::NoKey), static_cast<unsigned char>(0) )
{}

mouse_state::mouse_state()
{
  std::memset( buttons, 0, sizeof(buttons) );
}


}}

