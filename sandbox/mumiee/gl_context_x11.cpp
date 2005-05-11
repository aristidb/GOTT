#include "gl_context.hpp"
#include "globals_x11.hpp"

namespace gott{namespace gui{


gl_context::gl_context() 
  : os( new os_specific )
{
}

}}
