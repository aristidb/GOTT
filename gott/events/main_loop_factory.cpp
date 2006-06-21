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
 * The Original Code is An Event Handling Class Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
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

#include "main_loop_factory.hpp"
#include "loop_requirement.hpp"
#include "main_loop.hpp"
#include <gott/string/qid.hpp>
#include <gott/plugin.hpp>
#include <gott/exceptions.hpp>

using gott::events::main_loop_factory;
using gott::events::main_loop;

class main_loop_factory::impl {
public:
};

main_loop_factory::main_loop_factory() : p(new impl) {}
main_loop_factory::~main_loop_factory() {}

bool main_loop_factory::try_add(loop_requirement const &req) {
  return req.do_try(*this);
}

bool main_loop_factory::try_add_feature(QID const &) {
  return true; // FIXME
}

gott::plugin::selector main_loop_factory::get() const {
  return gott::plugin::with_interface_id("gott::events::main_loop");
}

/*
 * I think I shall sketch the way this class works or at least how I imagine it
 * to work.
 * First there is a std::list of all main_loop implementations in order of 
 * preference and their supported features (and stuff).
 * For every requested feature all implementations that do not support it are
 * removed from the list.
 * Except: If there is only one element on the list left, say no.
 *
 * This might need better thinking but this information should be valuable.
 */
