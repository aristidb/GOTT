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

#include "revocable_adapter.hpp"
#include <gott/tdl/structure/repatch.hpp>
#include <gott/tdl/source_position.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/optional.hpp>
#include <algorithm>
#include <vector>
#include <set>

//#define VERBOSE

#ifdef VERBOSE
#include <iostream>
#endif

using tdl::source_position;
using namespace tdl::structure;
using gott::Xany;
using gott::string;

namespace {
struct entry {
  enum type_t { begin, end, add_tag, data, add_repatcher2, remove_repatcher2 };

  type_t type;
  Xany param;
  source_position where;

  entry(type_t t, Xany p = Xany()) : type(t), param(p) {}
  entry(type_t t, source_position const &w) : type(t), where(w) {}

  void play(repatchable_structure2 &o) {
    switch (type) {
    case begin:
      o.begin(where);
      break;
    case end:
      o.end();
      break;
    case add_tag:
      o.add_tag(gott::Xany_cast<string>(param));
      break;
    case data:
      o.data(param);
      break;
    case add_repatcher2:
      o.add_repatcher2(
          gott::Xany_cast<boost::shared_ptr<repatcher const> >(param));
      break;
    case remove_repatcher2:
      o.remove_repatcher2(
          gott::Xany_cast<boost::shared_ptr<repatcher const> >(param));
      break;
    }
  }
};

const revocable_structure::pth nowhere = -1;
}

class revocable_adapter::impl {
public:
  repatchable_structure2 &out;
  pth pos;
  pth blocking;
  std::vector<entry> blocked;
  std::set<int> active;

  impl(repatchable_structure2 &o)
    : out(o), pos(0), blocking(nowhere) {}
};

revocable_adapter::revocable_adapter(repatchable_structure2 &out) 
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

void revocable_adapter::add_repatcher2(
    boost::shared_ptr<repatcher const> const &x) {
  if (p->blocking == nowhere)
    p->out.add_repatcher2(x);
  else
    p->blocked.push_back(entry(entry::add_repatcher2, Xany(x)));
  ++p->pos;
}

void revocable_adapter::remove_repatcher2(
    boost::shared_ptr<repatcher const> const &x) {
  if (p->blocking == nowhere)
    p->out.remove_repatcher2(x);
  else 
    p->blocked.push_back(entry(entry::remove_repatcher2, Xany(x)));
  ++p->pos;
}

revocable_structure::pth revocable_adapter::point() {
  if (p->blocking == nowhere)
    p->blocking = p->pos;
  p->active.insert(p->pos);
#ifdef VERBOSE
  std::cout << "point! " << p->pos << std::endl;
#endif
  return p->pos;
}

void revocable_adapter::revert(pth pp) {
#ifdef VERBOSE
  std::cout << "revert! " << pp << std::endl;
#endif
  size_t pp_blocked_size = pp - p->blocking;
  if (pp_blocked_size < p->blocked.size()) {
    std::vector<entry>::iterator begin = p->blocked.begin() + pp_blocked_size;
    std::vector<entry>::iterator end = p->blocked.end();
#ifdef VERBOSE
    for (std::vector<entry>::iterator it = begin; it != end; ++it) {
      std::cout << "removing output: ";
      switch (it->type) {
      case entry::begin: std::cout << "begin"; break;
      case entry::end: std::cout << "end"; break;
      case entry::add_tag: std::cout << "add_tag"; break;
      case entry::data: std::cout << "data"; break;
      case entry::add_repatcher: std::cout << "add_repatcher"; break;
      case entry::remove_repatcher: std::cout << "remove_repatcher"; break;
      }
      std::cout << std::endl;
    }
#endif
    p->blocked.erase(begin, end);
  }
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
