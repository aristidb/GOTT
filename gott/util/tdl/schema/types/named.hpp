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

#ifndef GOTT_UTIL_TDL_SCHEMA_NAMED_HPP
#define GOTT_UTIL_TDL_SCHEMA_NAMED_HPP

#include "../parse.hpp"
#include "../rule_factory.hpp"
#include "../rule_attr.hpp"
#include "../slot.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

class match_named : public rule {
public:
  static EXPORT
  rule::attributes attributes(std::wstring const &s, bool cc = true);
  
  typedef factory_template::slotcfg_onechild<
    match_named, slotcfg::one, slotcfg::single> factory;
  match_named(rule::factory const &, slotcfg const &, rule::attributes const &,
    match &);

  static bool accept_empty(bool) { return false; }

private:
  bool play(ev::node const &);
  bool play(ev::down const &);
  bool play(ev::up const &);
  bool play(ev::child_succeed const &);
  bool play(ev::child_fail const &);
  wchar_t const *name() const;
  
  rule::factory const &sub;
  bool optional;

  enum { read_none, read_node, read_down, read_sub, read_up } state;
};
  
}}}}

#endif
