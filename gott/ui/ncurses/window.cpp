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
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
 *  Aristid Breitkreuz (aribrei@arcor.de)
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

#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <ncurses.h>
#include <boost/bind.hpp> 
#include <boost/cstdint.hpp> 
#include <boost/lambda/lambda.hpp> 
#include <boost/lambda/bind.hpp> 
#include <boost/lambda/construct.hpp> 
#include <gott/exceptions.hpp>
#include <gott/ui/ncurses/window.hpp> 
#include <gott/ui/ncurses/screen_activator.hpp> 

using boost::bind;
using boost::lambda::var;
using boost::lambda::constructor;
using gott::properties::external_storage;
using gott::rect;
using gott::ui::window_base;
using gott::ui::ncurses::uicontext;
using gott::ui::ncurses::window;

window::window(
    uicontext &app,
    rect const &position,
    string const &title,
    std::size_t flags)
: region_(external_storage<rect>(
    bind(&window::get_region, this),
    bind(&window::set_region, this, _1))),
  title_(external_storage<gott::string>(
    var(title_string),
    bind(&window::set_title, this, _1))),
  visibility_(external_storage<bool>(
    var(mapped_state),
    bind(&window::map_window, this,_1))),
  flags_(external_storage<flags_type>(
    var(win_flags),
    bind(&window::set_window_type, this, _1))),
  window_(0),
  invalid_area(0,0,0,0),
  mapped_state(false),
  win_flags(0),
  context(&app)
{
  screen_activator activator(context);

  flags_.set(flags);

  title_.set(title);

  // flag this window as open
  flags |= window_flags::Open;

  window_ = ::newwin(
    position.width,
    position.height,
    position.left,
    position.top);

  if (!window_)
    throw system_error("Could not create a window.");

  region_.set(position);

  context->register_window(this);

  if (flags & window_flags::Visible)
    visibility_.set(true);

  wattron(window_, A_BOLD);
  wcolor_set(window_, 1, 0);
  wprintw(window_, "Hi There %p!!!", this);
  wattroff(window_, A_BOLD);
  wrefresh(window_);

  invalidate_area(rect(0,0,position.width, position.height));
}

rect window::get_region() const {
  return rect(0,0,0,0);
}

void window::handle_sys_resize(rect const& region) {
  if (region != last_region) {
    // resize 
    if (region.width != last_region.width ||
        region.height != last_region.height)
      invalidate_area(rect (0,0, region.width, region.height));
    last_region = region;
  }
}

void window::set_region(rect const& region) {
}

void window::set_title(gott::string const& str) {
  title_string = str;
}

void window::set_window_type(window_base::flags_type const &new_flags) {
}

gott::ui::window_base::rect_property_type& window::region() {
  return region_;
}

gott::ui::window_base::rect_property_type const& window::region() const {
  return region_;
}

gott::ui::window_base::string_property_type & window::title() {
  return title_;
}
 
gott::ui::window_base::string_property_type const& window::title() const {
  return title_;
}


gott::ui::window_base::toggle_property_type& window::visible() {
  return visibility_;
}
 
gott::ui::window_base::toggle_property_type const& window::visible() const {
  return visibility_;
}
 
gott::ui::window_base::flags_property_type& window::flags() {
  return flags_;
}
 
gott::ui::window_base::flags_property_type const& window::flags() const {
  return flags_;
}
 
void window::set_size_hints() {
}

void window::update_region(rect const& region) {
}

uicontext* window::get_uicontext() {
  return context;
}

bool window::needs_update() const {
  return invalid_area.width != 0 || invalid_area.height != 0;
}

rect window::get_invalid_area() const {
  return invalid_area;
}

void window::invalidate_area(rect const& region) {
  invalid_area.add_region(region);
}

void window::map_window(bool newstate) {
}

window::~window() {
  screen_activator activator(context);
  ::delwin(window_);
}

