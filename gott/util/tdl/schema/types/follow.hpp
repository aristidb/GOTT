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

#ifndef GOTT_TDL_SCHEMA_FOLLOW_HPP
#define GOTT_TDL_SCHEMA_FOLLOW_HPP

#include "../parse.hpp"
#include "../rule_factory.hpp"
#include "../slot.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

class match_follow : public rule {
public:
  typedef factory_template::somechildren<match_follow, 2> factory;
  match_follow(rule::factory const * const a[2], rule::attributes const &a, 
               match &m) EXPORT;
  
private:
  rule::factory const &pre, &post;
  enum { initial, pre_parsed, post_parsed, none } state;
  bool first_empty;

  bool play(ev::child_succeed const &);
  bool play(ev::down const &);
  bool play(ev::up const &);

  void post_done();
};

}}}}

#endif
