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

#include "gl_factory.hpp"
#include <GL/glx.h>
#include <gott/plugin/plugin_builder.hpp>
#include <gott/exceptions.hpp>

using gott::ui::x11::gl_factory; 
using gott::graphics::renderer; 

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(gott_ui_x11_gl_factory
    , gott::ui::x11::gl_factory)

gl_factory::~gl_factory()
{
}

::Visual* gl_factory::visual( ::Window rootX11WindowHandle, ::Display * display
    , int screen )
{
  int attributes[] = { GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 16, 
      GLX_DOUBLEBUFFER, True,  None};
  XVisualInfo * vInfo = glXChooseVisual( display, screen, attributes );
  if(! vInfo ) 
    throw system_error("No X Visual for OpenGL found.");
  return vInfo->visual;
}

renderer * gl_factory::renderer( ::Window x11WindowHandle, ::Display * display
    , int screen )
{
  return 0;
}

int gl_factory::depth( ::Window rootX11WindowHandle, ::Display * display
    , int screen )
{
  int attributes[] = { GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 16, 
      GLX_DOUBLEBUFFER, True,  None};
  XVisualInfo * vInfo = glXChooseVisual( display, screen, attributes );
  if(! vInfo ) 
    throw system_error("No X Visual for OpenGL found.");
  return vInfo->depth;
}

