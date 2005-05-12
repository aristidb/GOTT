#include "gl_context.hpp"
#include "globals_x11.hpp"

namespace gott{namespace gui{

gl_context::os_specific::os_specific()
  :  handle( None )
{
}

gl_context::os_specific::~os_specific()
{
  glXDestroyContext( global_data.connection, handle );
}

gl_context::gl_context() 
  : os( new os_specific )
{
}

}}
