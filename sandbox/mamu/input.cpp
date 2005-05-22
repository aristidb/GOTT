#include "input.hpp"

namespace gott{namespace gui{
key_event::key_event( key_code c, event_type t)  
    : type(t), code(c)
{}

key_state::key_state()
{}


}}

