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
 * The Original Code is an OpenGL based renderer.
 *
 * The Initial Developer of the Original Code is
 * Andreas Pokorny (andreas.pokorny@gmail.com)
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include <gott/graphics/gl/renderer.hpp>
#include <iostream>

using gott::graphics::gl::renderer;
using gott::graphics::gl::context;

renderer::renderer(context * con)
  : context_(con)
{
  std::cout << "gl::renderer::renderer( " << con << " );" << std::endl;
}

void renderer::resize( gott::rect const& draw_area ) 
{
  context_->make_current();
  std::cout << "gl::renderer::resize( " << draw_area << " );" << std::endl;
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glViewport( 0, 0, draw_area.width, draw_area.height );
  glOrtho( 0, draw_area.width, 0, draw_area.height, -1, 1 );
  glDisable( GL_DEPTH_TEST );
  glDisable( GL_CULL_FACE );

}



void renderer::update_display(rect const& invalid_region )
{
  context_->make_current();
  std::cout << "gl::renderer::update_display( " << invalid_region << " );" << std::endl;
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0, invalid_region.width,  invalid_region.height, 0, -1, 1 );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glClear(GL_COLOR_BUFFER_BIT);
  
  glBegin( GL_QUADS );
    glColor3ub( 255,0,0); glVertex2i( 0, 0);
    glColor3ub( 0,255,0); glVertex2i( 0, 100 );
    glColor3ub( 0,0,255); glVertex2i( 100, 100 );
    glColor3ub( 255,0,255); glVertex2i( 100, 0 );
  glEnd();

  glFlush();
  context_->swap_buffers();
}

