// Copyright (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "revocable_adapter.hpp"
#include <vector>

using gott::tdl::structure::revocable_adapter;
using gott::tdl::structure::revocable_structure;
using gott::tdl::structure::writable_structure;
using gott::xany::Xany;
using gott::string;

namespace {
struct entry {
  enum type_t { begin, end, add_tag, data } type;
  Xany param;
  entry(type_t t, Xany p = Xany()) : type(t), param(p) {}
  void play(writable_structure &o) {
    switch (type) {
    case begin: o.begin(); break;
    case end: o.end(); break;
    case add_tag: o.add_tag(gott::xany::Xany_cast<string>(param)); break;
    case data: o.data(param); break;
    }
  }
};

const revocable_structure::pth nowhere = -1;
}

class revocable_adapter::IMPL {
public:
  writable_structure &out;
  pth pos;
  pth blocking;
  std::vector<entry> blocked;
  IMPL(writable_structure &o) : out(o), pos(0), blocking(nowhere) {}
};

revocable_adapter::revocable_adapter(writable_structure &out) 
: p(new IMPL(out)) {}
revocable_adapter::~revocable_adapter() {}

void revocable_adapter::begin() {
  if (p->blocking == nowhere)
    p->out.begin();
  else
    p->blocked.push_back(entry::begin);
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
  return p->pos;
}

void revocable_adapter::revert(pth pp) {
  size_t pp_blocked_size = pp - p->blocking;
  if (pp_blocked_size < p->blocked.size())
    p->blocked.erase(p->blocked.begin() + pp_blocked_size, p->blocked.end());
  p->pos = pp;
}

void revocable_adapter::get_rid_of(pth pp) {
  if (pp == p->blocking) {
    p->blocking = nowhere;
    for (std::vector<entry>::iterator it = p->blocked.begin(); 
        it != p->blocked.end(); ++it)
      it->play(p->out);
    std::vector<entry>().swap(p->blocked);
  }
}
