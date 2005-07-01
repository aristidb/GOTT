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
  bool replay, in_replay;

  IMPL(revocable_structure &s) 
    : struc(s), unconsumed(0), consumed(0), replay(false) {}
};

positioning::positioning(revocable_structure &s) : p(new IMPL(s)) {}
positioning::~positioning() {}

template<class T>
void positioning::add(T const &t) {
  p->unconsumed = p->buffer.size();
  p->buffer.push_back(t);
}

template void positioning::add(ev::begin_parse const &);
template void positioning::add(ev::end_parse const &);
template void positioning::add(ev::down const &);
template void positioning::add(ev::up const &);
template void positioning::add(ev::node const &);

void positioning::consume() {
  p->consumed = p->unconsumed + p->in_replay;
}

bool positioning::proceeded(id const &x) const {
  return x.first < p->consumed;
}

unsigned positioning::debug_current() const {
  return p->unconsumed;
}

positioning::id positioning::current() {
  return std::make_pair(p->unconsumed, p->struc.point());
}

positioning::id positioning::next() {
  return std::make_pair(p->unconsumed + 1, p->struc.point());
}

void positioning::seek(id const &x) {
  p->consumed = p->seeked = x.first;
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
    p->in_replay = true;
    for (p->unconsumed = p->seeked;
         p->unconsumed < p->buffer.size();
         ++p->unconsumed) {
      acc(get(p->buffer[p->unconsumed]));
      if (p->replay)
        break;
    }
    p->in_replay = false;
  }
}

bool positioning::want_replay() const {
  return p->replay;
}

void positioning::debug_dump() const {
  std::wcout << L" [";
  std::wcout << L'U' << p->unconsumed << L',';
  std::wcout << L'C' << p->consumed << L',';
  std::wcout << L'S' << p->seeked << L',';
  std::wcout << L'R' << p->replay << L',';
  std::wcout << L'I' << p->in_replay;
  std::wcout << L"]\n";
}
