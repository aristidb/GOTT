// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is a user interface abstraction library.
 *
 * The Initial Developer of the Original Code is
 * Andreas Pokorny (andreas.pokorny@gmail.com)
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "glx_factory.hpp"
#include <GL/glx.h>
#include <gott/plugin/plugin_builder.hpp>
#include <gott/exceptions.hpp>
#include <gott/graphics/gl/renderer.hpp>

using gott::ui::x11::glx_factory; 
using gott::ui::x11::glx_resource;
using gott::graphics::renderer; 

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(plugin , gott::ui::x11::glx_factory)

glx_factory::~glx_factory()
{
}

::GLXFBConfig glx_factory::fb_config( ::Display * display, int screen )
{
  // possible options could be 16bit vs 32bit, Stencil on/off, alpha on/off, 
  int attributes[] = { GLX_DRAWABLE_TYPE_SGIX, GLX_WINDOW_BIT_SGIX
    , GLX_DOUBLEBUFFER, 1, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8
      , GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 16
      , GLX_DOUBLEBUFFER, True,  None};

  int size = 0;

  GLXFBConfig * config = glXChooseFBConfig( display, screen, attributes, &size);
  if( 0 == size ) 
    throw system_error("No X Visual for OpenGL found.");

  // fist best? config is chosen 
  return *config; 
}

::XVisualInfo* glx_factory::visual_info( ::Window rootX11WindowHandle
    , ::Display * display, int screen )
{
  return glXGetVisualFromFBConfig( display, fb_config(display, screen) );
}

renderer * glx_factory::renderer( ::Window x11WindowHandle, ::Display * display
    , int screen )
{
  return new gott::graphics::gl::renderer( 
      new glx_resource( 
        fb_config( display, screen ) 
        , display
        , x11WindowHandle 
        ) 
      ) ;
}


glx_resource::glx_resource( GLXFBConfig config, ::Display *display
    , ::Window windowHandle )
  : window_(0), context_(0), display_(display)
{
  window_ = glXCreateWindow( display, config, windowHandle, 0 );
  context_ = glXCreateNewContext( display, config, GLX_RGBA_TYPE, 0, 1 );
}

glx_resource::~glx_resource()
{
  glXDestroyWindow( display_, window_ );
  glXDestroyContext( display_, context_ );

}

void glx_resource::make_current()
{
  if( ! glXMakeContextCurrent( display_, window_, window_, context_ ) )
    throw gott::system_error( "Failure during make_current" );
}
