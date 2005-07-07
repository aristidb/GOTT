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

#ifndef GOTT_UTIL_TDL_SCHEMA_UNORDERED_HPP
#define GOTT_UTIL_TDL_SCHEMA_UNORDERED_HPP

#include "../parse.hpp"
#include "../rule_factory.hpp"
#include "../parse_position.hpp"
#include "../slot.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

class match_unordered : public rule {
  typedef std::pair<rule::factory const *, slotcfg> element;
public:
  typedef factory_template::slotcfg_manychildren<
            match_unordered,
            slotcfg::one,
            slotcfg::all>
          factory;
  match_unordered(std::vector<element> const &, rule::attributes const &, 
                  match &);
  ~match_unordered();

private:
  typedef std::list<element> list_t;
  list_t children;
  list_t::iterator pos;
  positioning::id last;

  bool play(ev::child_succeed const &);
  bool play(ev::child_fail const &);
};

}}}}

#endif
