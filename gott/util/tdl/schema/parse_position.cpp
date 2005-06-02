// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

#include "parse_position.hpp"
#include "event.hpp"
#include <gott/util/tdl/structure/structure.hpp>

using std::vector;
using std::size_t;
namespace schema = gott::util::tdl::schema;
namespace structure = gott::util::tdl::structure;

using structure::revocable_structure;
using schema::positioning;

class positioning::IMPL {
public:
  typedef vector<ev::token_t> buffer_t;

  revocable_structure &struc;

  buffer_t buffer;
  size_t unconsumed, consumed, seeked;
  bool replay;

  IMPL(revocable_structure &s) 
    : struc(s), unconsumed(0), consumed(0), replay(false) {}
};

positioning::positioning(revocable_structure &s) : p(new IMPL(s)) {}
positioning::~positioning() {}

template<class T>
void positioning::add(T const &t) {
  p->buffer.push_back(t);
  p->unconsumed = p->buffer.size();
}

template void positioning::add(ev::begin_parse const &);
template void positioning::add(ev::end_parse const &);
template void positioning::add(ev::down const &);
template void positioning::add(ev::up const &);
template void positioning::add(ev::node const &);

void positioning::consume() {
  p->consumed = p->unconsumed;
}

bool positioning::proceeded(id const &x) const {
  return x.first < p->consumed;
}

positioning::id positioning::current() {
  return std::make_pair(p->unconsumed, p->struc.point());
}

void positioning::seek(id const &x) {
  p->seeked = x.first;
  p->struc.revert(x.second);
  p->replay = true;
}

void positioning::forget(id const &x) {
  p->struc.get_rid_of(x.second);
}

void positioning::seek_and_forget(id const &x) {
  seek(x);
  forget(x);
}

void positioning::replay(acceptor &acc) {
  if (p->replay) {
    p->replay = false;
    for (p->unconsumed = p->seeked;
         p->unconsumed < p->buffer.size();
         ++p->unconsumed) {
      acc(get(p->buffer[p->unconsumed]));
      if (p->replay)
        break;
    }
  }
}

bool positioning::want_replay() const {
  return p->replay;
}
