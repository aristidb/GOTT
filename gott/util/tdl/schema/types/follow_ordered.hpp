// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#ifndef GOTT_TDL_SCHEMA_FOLLOW_ORDERED_HPP
#define GOTT_TDL_SCHEMA_FOLLOW_ORDERED_HPP

#include "../match.hpp"
#include "../rule.hpp"
#include "../slot.hpp"
#include "../parse_position.hpp"

namespace gott {
namespace tdl {
namespace schema {

class match_follow_ordered : public item {
  typedef std::pair<rule const *, slotcfg> element;
public:
  match_follow_ordered(Vector<element> const &, rule_attr const &, 
                       match &);
  ~match_follow_ordered();

  static bool accept_empty(Vector<element> const &);
  
private:
  struct active_element : Moveable<active_element> {
    rule const *generator;
    slotcfg slot;
    bool accept_empty, rest_accept_empty;
    
    active_element(element const &e) 
      : generator(e.first), slot(e.second) {}
  };

  void init_accept_empty();
  bool search_insertible() const;

  typedef Vector<active_element> container;
  container children;
  mutable container::iterator pos;
  int opened;
  bool saw_up;
  positioning::id last;
  bool unhappy;

  expect expectation() const;
  bool play(ev::child_succeed const &);
  bool play(ev::child_fail const &);
  bool play(ev::down const &);
  bool play(ev::up const &);
  bool miss_events(ev::event const &, unsigned);
  string name() const;
};

}}}

#endif
