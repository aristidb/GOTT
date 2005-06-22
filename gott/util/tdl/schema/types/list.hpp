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

#ifndef GOTT_UTIL_TDL_SCHEMA_LIST_HPP
#define GOTT_UTIL_TDL_SCHEMA_LIST_HPP

#include "../parse.hpp"
#include "../rule_factory.hpp"
#include "../slot.hpp"
#include "../parse_position.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

class match_list : public rule {
public:
  typedef factory_template::slotcfg_onechild<match_list, 
                                             slotcfg::list, 
                                             slotcfg::all>
          factory;

  match_list(rule::factory const &, slotcfg const &, rule::attributes const &,
                  match &);
  ~match_list();

private:
  rule::factory const &sub;
  positioning::id last;
  slotcfg cfg;

  bool play(ev::child_fail const &);
  bool play(ev::child_succeed const &);
  bool full();
  bool empty();
};

}}}}
#endif
