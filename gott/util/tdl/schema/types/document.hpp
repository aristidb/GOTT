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

#ifndef GOTT_UTIL_TDL_SCHEMA_DOCUMENT_HPP
#define GOTT_UTIL_TDL_SCHEMA_DOCUMENT_HPP

#include "../parse.hpp"
#include "../rule_factory.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

// Matcher document
// Matches a whole tdl document that contains the given sub-type
class match_document : public rule {
public:
  typedef factory_template::onechild<match_document> factory;
  match_document(rule::factory const &, rule::attributes const &, match &);

  static bool accept_empty(...) { return false; }
  
private:
  rule::factory const &sub;
  enum { first, begun_parse, opened, closed } state;

  bool play(ev::begin_parse const &);
  bool play(ev::down const &);
  bool play(ev::up const &);
  bool play(ev::end_parse const &);
  bool play(ev::child_succeed const &);
  wchar_t const *name() const;
};

}}}}
#endif
