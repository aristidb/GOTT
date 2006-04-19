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
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
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

#include "parse_position.hpp"
#include "event.hpp"
#include <gott/tdl/source_position.hpp>
#include <gott/tdl/structure/structure.hpp>
#include <vector>
#include <set>
#include <cstddef>

using std::size_t;
namespace schema = tdl::schema;
namespace structure = tdl::structure;

using structure::revocable_structure;
using schema::positioning;

class positioning::impl {
public:
  typedef std::vector<std::pair<ev::token_t, source_position> > buffer_t;

  revocable_structure &struc;

  buffer_t buffer_real;
  std::set<std::size_t> blocks;

  buffer_t::value_type &buffer(unsigned long pos) {
    return buffer_real[pos - base];
  }

  unsigned long buffer_size() {
    return buffer_real.size() + base;
  }

  template<class T>
  void buffer_add(T const &x, source_position const &w) {
    buffer_real.push_back(buffer_t::value_type(x, w));
  }
  
  void add_block(std::size_t p) {
    blocks.insert(p);
  }

  void remove_block(std::size_t p) {
    blocks.erase(p);
  }

  void clean_blocked() {
    if (blocks.empty()) {
      base += buffer_real.size();
      buffer_real.clear();
    }
  }
  
  long unconsumed, consumed, seeked;
  bool replay, in_replay;
  bool in_pass;
  unsigned long base;

  long current() {
    if (in_pass)
      return consumed;
    else
      return unconsumed;
  }

  impl(revocable_structure &s)
  : struc(s), unconsumed(0), consumed(-1), replay(false), in_pass(false), 
    base(0) {}
};

positioning::positioning(revocable_structure &s) : p(new impl(s)) {}
positioning::~positioning() {}

template<class T>
void positioning::add(T const &t, source_position const &w) {
  p->unconsumed = p->buffer_size();
  p->buffer_add(t, w);
  p->in_pass = false;
}

template void positioning::add(ev::begin_parse const &, source_position const &);
template void positioning::add(ev::end_parse const &, source_position const &);
template void positioning::add(ev::down const &, source_position const &);
template void positioning::add(ev::up const &, source_position const &);
template void positioning::add(ev::node const &, source_position const &);

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
  p->add_block(p->current());
  return id(p->current(), p->struc.point());
}

positioning::id positioning::current_readonly() {
  return id(p->current(), -1);
}

void positioning::seek(id const &x) {
  p->seeked = x.first;
  p->struc.revert(x.second);
  p->replay = true;
}

void positioning::forget(id const &x) {
  p->remove_block(x.first);
  p->struc.get_rid_of(x.second);
}

void positioning::seek_and_forget(id const &x) {
  seek(x);
  forget(x);
}

void positioning::replay(acceptor const &acc) {
  if (p->replay) {
    p->replay = false;
    p->in_replay = true;
    p->in_pass = false;
    p->consumed = p->seeked;
    for (p->unconsumed = p->consumed + 1;
         p->unconsumed < long(p->buffer_size());
         ++p->unconsumed) {
      impl::buffer_t::value_type &x = p->buffer(p->unconsumed);
      acc(get(x.first), x.second);
      if (p->replay)
        break;

      assert(p->consumed == p->unconsumed);
    }
    p->in_replay = false;
  } else
    p->clean_blocked();
}

bool positioning::want_replay() const {
  return p->replay;
}

#ifdef DEBUG
#include <iostream>

void positioning::debug_dump() const {
  std::cerr << "[";
  std::cerr << 'U' << p->unconsumed << ',';
  std::cerr << 'C' << p->consumed << ',';
  std::cerr << 'S' << p->seeked << ',';
  std::cerr << 'R' << p->replay << ',';
  std::cerr << 'I' << p->in_replay;
  std::cerr << "]";
}
#endif
