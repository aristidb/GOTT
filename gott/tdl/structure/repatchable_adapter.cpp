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
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include "repatchable_adapter.hpp"
#include <gott/tdl/structure/repatch.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#ifndef NDEBUG
#include <vector>
#include <cassert>
#endif

using namespace tdl::structure;

class repatchable_adapter::impl {
public:
  writable_structure &out;
  boost::ptr_vector<writable_structure> indirections;
  std::vector<boost::shared_ptr<repatcher const> > tracker;

  writable_structure &current() {
    if (indirections.empty())
      return out;
    else
      return indirections.back();
  }

  impl(writable_structure &out) : out(out) {}
};

repatchable_adapter::repatchable_adapter(writable_structure &out)
  : p(new impl(out)) {}
repatchable_adapter::~repatchable_adapter() {}

void repatchable_adapter::begin(tdl::source_position const &w) {
  p->current().begin(w);
}

void repatchable_adapter::end() {
  p->current().end();
}

void repatchable_adapter::add_tag(gott::string const &s) {
  p->current().add_tag(s);
}

void repatchable_adapter::data(gott::Xany const &x) {
  p->current().data(x);
}

void repatchable_adapter::add_repatcher(
    boost::shared_ptr<repatcher const> const &x) {
  p->tracker.push_back(x);
  writable_structure &old = p->current();
  writable_structure *fresh = x->deferred_write(old);
  p->indirections.push_back(fresh);
}

void repatchable_adapter::remove_repatcher(
    boost::shared_ptr<repatcher const> const &x) {
#ifndef NDEBUG
  assert(!p->tracker.empty());
  assert(p->tracker.back() == x);
#else
  (void)x;
#endif
  p->tracker.pop_back();
  assert(!p->indirections.empty());
  p->indirections.pop_back();
}
