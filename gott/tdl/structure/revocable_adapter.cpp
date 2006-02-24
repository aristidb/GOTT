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

#include "revocable_adapter.hpp"
#include <gott/tdl/source_position.hpp>
#include <vector>
#include <set>

using tdl::source_position;
using tdl::structure::revocable_adapter;
using tdl::structure::revocable_structure;
using tdl::structure::writable_structure;
using gott::xany::Xany;
using gott::string;

namespace {
struct entry {
  enum type_t { begin, end, add_tag, data } type;
  Xany param;
  source_position where;
  entry(type_t t, Xany p = Xany()) : type(t), param(p) {}
  entry(type_t t, source_position const &w) : type(t), where(w) {}
  void play(writable_structure &o) {
    switch (type) {
    case begin: o.begin(where); break;
    case end: o.end(); break;
    case add_tag: o.add_tag(gott::xany::Xany_cast<string>(param)); break;
    case data: o.data(param); break;
    }
  }
};

const revocable_structure::pth nowhere = -1;
}

class revocable_adapter::impl {
public:
  writable_structure &out;
  pth pos;
  pth blocking;
  std::vector<entry> blocked;
  std::set<int> active;
  impl(writable_structure &o) : out(o), pos(0), blocking(nowhere) {}
};

revocable_adapter::revocable_adapter(writable_structure &out) 
: p(new impl(out)) {}
revocable_adapter::~revocable_adapter() {}

void revocable_adapter::begin(source_position const &w) {
  if (p->blocking == nowhere)
    p->out.begin(w);
  else
    p->blocked.push_back(entry(entry::begin, w));
  ++p->pos;
}

void revocable_adapter::end() {
  if (p->blocking == nowhere)
    p->out.end();
  else
    p->blocked.push_back(entry::end);
  ++p->pos;
}

void revocable_adapter::add_tag(string const &t) {
  if (p->blocking == nowhere)
    p->out.add_tag(t);
  else
    p->blocked.push_back(entry(entry::add_tag, Xany(t)));
  ++p->pos;
}

void revocable_adapter::data(Xany const &x) {
  if (p->blocking == nowhere)
    p->out.data(x);
  else
    p->blocked.push_back(entry(entry::data, x));
  ++p->pos;
}

revocable_structure::pth revocable_adapter::point() {
  if (p->blocking == nowhere)
    p->blocking = p->pos;
  p->active.insert(p->pos);
  return p->pos;
}

void revocable_adapter::revert(pth pp) {
  size_t pp_blocked_size = pp - p->blocking;
  if (pp_blocked_size < p->blocked.size())
    p->blocked.erase(p->blocked.begin() + pp_blocked_size, p->blocked.end());
  p->pos = pp;
}

void revocable_adapter::get_rid_of(pth pp) {
  p->active.erase(pp);
  if (p->active.empty()) {
    p->blocking = nowhere;
    for (std::vector<entry>::iterator it = p->blocked.begin(); 
        it != p->blocked.end(); ++it)
      it->play(p->out);
    std::vector<entry>().swap(p->blocked);
  }
}
