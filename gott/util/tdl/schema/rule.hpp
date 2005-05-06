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

#ifndef GOTT_UTIL_TDL_SCHEMA_RULE_HPP
#define GOTT_UTIL_TDL_SCHEMA_RULE_HPP

#include <gott/util/misc/commonheaders.hpp>
#include "ev_fwd.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

class match;

// Abstract rule
// defines a rule object that can be matched
// "match" feeds us with events and other notifications
class EXPORT rule {
public:
  struct factory;
  class attributes;

  enum expect { nothing = 1, maybe = 2, need = -3, over_filled = -4 };
    // a rule may:
    // expect <nothing> (when it's finished or empty)
    // <maybe> accept something (when there is optional input it can be feeded)
    // <need> input
    // be <over_filled> (should not happen - it usually can ignore input)
  
  expect expects() const { return expectation; }
    // what do we expect?


  // Event handlers
  //  bool play(ev::event const &ev)
    // try to match a token (or "event")
    // return value: true on success
    // side effects: adjust the expectation as necessary

    // implemented as: visitor pattern -->
    // default implementation: do nothing
  
  // Tokens
  virtual bool play(ev::begin_parse const &) EXPORT;
  virtual bool play(ev::down const &) EXPORT;
  virtual bool play(ev::node const &) EXPORT;
  virtual bool play(ev::up const &) EXPORT;
  virtual bool play(ev::end_parse const &) EXPORT;
  // Notifications
  virtual bool play(ev::child_succeed const &) EXPORT;
  virtual bool play(ev::child_fail const &) EXPORT;

  void finish();
  
  virtual ~rule() EXPORT = 0;

  // Properties
  attributes const &get_attributes() const EXPORT;
    // get the attributes associated with this rule

  //virtual wchar_t const *name() const EXPORT = 0;
    // well, mangled typeid() names are hard to read!

protected:
  match &matcher() EXPORT;
    // return a reference to the match-object that embedded us

  rule(expect e, attributes a, match &m) EXPORT;
    // an implementation must supply attributes and its initial expectation

  expect expectation;

private:
  class IMPL;
  IMPL *pIMPL;
};

}}}}

#endif
