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
  long unconsumed, consumed, seeked;
  bool replay, in_replay;
  bool in_pass;

  IMPL(revocable_structure &s) 
    : struc(s), unconsumed(0), consumed(-1), replay(false), in_pass(false) {}
};

positioning::positioning(revocable_structure &s) : p(new IMPL(s)) {}
positioning::~positioning() {}

template<class T>
void positioning::add(T const &t) {
  p->unconsumed = p->buffer.size();
  p->buffer.push_back(t);
  p->in_pass = false;
}

template void positioning::add(ev::begin_parse const &);
template void positioning::add(ev::end_parse const &);
template void positioning::add(ev::down const &);
template void positioning::add(ev::up const &);
template void positioning::add(ev::node const &);

void positioning::consume() {
  ++p->consumed;
}

void positioning::pass() {
  p->in_pass = true;
}

bool positioning::proceeded(id const &x) const {
  return x.first < p->consumed;
}

positioning::id positioning::current() {
  if (p->in_pass)
    return std::make_pair(p->consumed, p->struc.point());
  else
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
    p->in_replay = true;
    p->in_pass = false;
    p->consumed = p->seeked;
    for (p->unconsumed = p->consumed + 1;
         p->unconsumed < long(p->buffer.size());
         ++p->unconsumed) {
      acc(get(p->buffer[p->unconsumed]));
      if (p->replay)
        break;
      GOTT_ASSERT_2(p->consumed, p->unconsumed, std::equal_to<long>(), 
                    "Gotta consume token");
    }
    p->in_replay = false;
  }
}

bool positioning::want_replay() const {
  return p->replay;
}

#ifdef DEBUG
void positioning::debug_dump() const {
  std::wcerr << L"[";
  std::wcerr << L'U' << p->unconsumed << L',';
  std::wcerr << L'C' << p->consumed << L',';
  std::wcerr << L'S' << p->seeked << L',';
  std::wcerr << L'R' << p->replay << L',';
  std::wcerr << L'I' << p->in_replay;
  std::wcerr << L"]";
}
#endif
