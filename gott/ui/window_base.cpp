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

#include <gott/ui/window_base.hpp>

using gott::ui::window_base;
using gott::ui::mouse_event;
using gott::ui::key_event;
using gott::rect;

window_base::~window_base()
{}


boost::signal1<void, rect const&> & window_base::on_configure()
{
  return configure_;
}

boost::signal1<void, rect const&> & window_base::on_resize()
{
  return resize_;
}

boost::signal1<void, rect const&> & window_base::on_move()
{
  return move_;
}

boost::signal0<void> & window_base::on_focus_enter()
{
  return focus_enter_;
}

boost::signal0<void> & window_base::on_close()
{
  return close_;
}

boost::signal0<void> & window_base::on_focus_leave()
{
  return focus_leave_;
}

boost::signal1<void,mouse_event const&> & window_base::on_mouse()
{
  return mouse_;
}

boost::signal1<void,key_event const&> & window_base::on_key()
{
  return key_;
}

