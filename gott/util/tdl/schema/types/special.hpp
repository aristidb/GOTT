// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
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

#ifndef GOTT_UTIL_TDL_SCHEMA_SPECIAL_HPP
#define GOTT_UTIL_TDL_SCHEMA_SPECIAL_HPP

#include "../rule.hpp"
#include "../rule_factory.hpp"
#include "../rule_attr.hpp"
#include "../happy_once.hpp"
#include <utility>

namespace gott {
namespace tdl {
namespace schema {

class match_special : public happy_once {
public:
  static rule_attr attributes(unsigned down, unsigned up) {
    std::pair<unsigned, unsigned> data(down, up);
    return rule_attr(Vector<string>(), false, xany::Xany(data));
  }

  typedef factory_template::nochild<match_special> factory;
  match_special(rule_attr const &, match &);

  static bool accept_empty() { return false; }
  
private:
  bool play(ev::down const &);
  bool play(ev::up const &);

  unsigned down_left, up_left;

  string name() const;
};

}}}

#endif
