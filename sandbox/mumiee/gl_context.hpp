
#ifndef GOTT_GUI_GL_CONTEXT_HPP_INCLUDED
#define GOTT_GUI_GL_CONTEXT_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

namespace gott{namespace gui{

struct gl_context
{
  struct os_specific;
  boost::shared_ptr<os_specific> os;
  gl_context();
};

  
}}

#endif

