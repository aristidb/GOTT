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
 * The Original Code is a set of geometry classes.
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

#include <gott/graphics/geometry.hpp>
#include <utility>
#include <algorithm>

namespace gott{

void rect::add_region( rect const& other ) 
{
  width = std::max( left + width, other.left + other.width);
  left = std::min( left, other.left );
  height = std::max( top + height, other.top + other.height);
  top = std::min( top, other.top);
}

void rect::subtract_region( rect const& other ) {
  if( left <= other.left && top <= other.top && left+width <= other.left+other.width && top+height <= other.top+other.height) 
    left = top = width = height = 0 ;
  else {
    if( other.left <= left && (other.left + other.width) >= (left + width) ) {
      if( other.top <= top &&  (other.top+long(other.height)) >= top ){
        long old_tp = top + long(height);
        top = std::min( other.top + long(other.height), old_tp );
        height = old_tp - top;
      }
      else if ( other.top + other.height > (top+height) && other.top <= (top+long(height))  ) 
      {
        height = other.top - top;
      }
    else if( other.top <= top
        && (other.top + other.height) >= (top + height) ) {
      if ( other.left <= left &&  (other.left+long(other.width)) >= left ){
        long old_lw = left + long(width);
        left = std::min( other.left+long(other.width), old_lw );
        width = old_lw - left;
      }
      else if ( other.left+other.width> (left+width) && other.left <= (left+long(width))  ) 
        width = other.left-width;
      }
    }
  }
}
}

