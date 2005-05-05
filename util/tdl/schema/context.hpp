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

#ifndef GOTT_UTIL_TDL_SCHEMA_CONTEXT_HPP
#define GOTT_UTIL_TDL_SCHEMA_CONTEXT_HPP

#include <gott/util/misc/commonheaders.hpp>
#include "rule_factory.hpp"
#include "slot.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

class context : public boost::noncopyable {
public:
  context() EXPORT;
  ~context() EXPORT;

  void EXPORT begin(unsigned rule_id, 
                    rule::attributes const &attr = rule::attributes(),
                    boost::optional<slotcfg> const & = boost::none,
                    unsigned = 0);
  void end() EXPORT;
  
  template<class T> 
  void begin_t(rule::attributes const &attr, unsigned c = 0) {
    begin(T::factory::index(), attr, c);
  }
  
  void ref(context const &other) EXPORT;
  rule::factory const &get() const EXPORT;

private:
  typedef std::vector<rule::factory *> container;
  container pool;
  rule::factory const *first;
  std::list<rule::factory *> org;

  void add_child(rule::factory const *, boost::optional<slotcfg> const &);
  void add_slotted(rule::factory const *, slotcfg const &);
  void add_enc_slotted(rule::factory const *, slotcfg const &, 
                       boost::optional<slotcfg> const & = boost::none);
  void add_owned(rule::factory *, boost::optional<slotcfg> const &);
};

class multi_context {
public:
  multi_context(boost::shared_ptr<context> cc, boost::shared_ptr<multi_context>
                nn = boost::shared_ptr<multi_context>())
  : c(cc), n(nn) {}
  
  context &get() const { return *c; }
  boost::shared_ptr<multi_context> next() const { return n; }

  template<class I> 
  friend boost::shared_ptr<multi_context> build_multi_context(I,I);
  
private:
  boost::shared_ptr<context> c;
  boost::shared_ptr<multi_context> n;
};

typedef boost::shared_ptr<multi_context> multi_context_handle;

// Build a multi_context from a range of contexts given by two iterators
// Requirement: start > stop
template<class I>
multi_context_handle build_multi_context(I start, I stop) {
  multi_context_handle res;

  for (; start != stop; ++start)
    res = multi_context_handle(new multi_context(*start, res));

  return res;
}

inline multi_context_handle binary_combine(boost::shared_ptr<context> a, 
                                           boost::shared_ptr<context> b) {
  multi_context_handle rhs(new multi_context(b));
  return multi_context_handle(new multi_context(a, rhs));
}

inline multi_context_handle cons(boost::shared_ptr<context> a,
                                 multi_context_handle b) {
  return multi_context_handle(new multi_context(a, b));
}

inline context &car(multi_context_handle x) {
  return x->get();
}

inline multi_context_handle cdr(multi_context_handle x) {
  return x->next();
}

}}}}

#endif
