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

#ifndef GOTT_UTIL_TDL_SCHEMA_CONTEXT_HPP
#define GOTT_UTIL_TDL_SCHEMA_CONTEXT_HPP

#include <gott/util/misc/commonheaders.hpp>
#include "rule_factory.hpp"
#include "slot.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

/**
 * Memory-owning context class.
 */
class context : public boost::noncopyable {
public:
  context() GOTT_EXPORT;
  ~context() GOTT_EXPORT;

  /**
   * Begin the declaration of a "rule".
   * The context class owns its memory.
   * \param rule_id The rule-factory builder's index.
   * \param attr The rule-factory's and later rule's attributes.
   * \param slt (optional) Repetition definition.
   */
  void begin(unsigned rule_id,
             rule_attr const &attr = rule_attr(),
             boost::optional<slotcfg> const &slt = boost::none);

  /**
   * Begin the declaration of a "rule".
   * The context class owns its memory.
   * \param name The type's name.
   * \param attr The rule-factory's and later rule's attributes.
   * \param slt (optional) Repetition definition.
   */
  void GOTT_EXPORT begin(std::wstring const &name,
                    rule_attr const &attr = rule_attr(),
                    boost::optional<slotcfg> const &slt = boost::none);

  /**
   * End the declaration of a "rule".
   * @see begin
   */
  void end() GOTT_EXPORT;

  /**
   * A little convenience wrapper around begin.
   */
  template<class T>
  void begin_t(rule_attr const &attr, unsigned c = 0) {
    begin(T::factory::index(), attr, c);
  }

  /**
   * Add a reference to another context as a "rule".
   * The context must be fully initialized and filled.
   * The added context is not owned.
   * \param other The context to refer to.
   */
  void ref(context const &other) GOTT_EXPORT;

  /**
   * Instantiate the context.
   */
  rule_factory const &get() const GOTT_EXPORT;

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

/**
 * A class for combining context objects.
 * Pretty much untested yet.
 */
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

/**
 * Build a multi_context from a range of contexts given by two iterators
 * \param start The start of the range.
 * \param stop The end of the range.
 * \return A smart pointer (handle) to the multi_context.
 * @invariant start < stop
 */
template<class I>
multi_context_handle build_multi_context(I start, I stop) {
  multi_context_handle res;

  for (; start != stop; ++start)
    res = multi_context_handle(new multi_context(*start, res));

  return res;
}

/**
 * Combine two contexts to form a multi_context.
 * \param a One context smart-pointer.
 * \param b Another context smart-pointer.
 * \return A smart pointer (handle) to the multi_context.
 */
inline multi_context_handle binary_combine(boost::shared_ptr<context> a,
                                           boost::shared_ptr<context> b) {
  multi_context_handle rhs(new multi_context(b));
  return multi_context_handle(new multi_context(a, rhs));
}

/**
 * Combine a context with a multi_context.
 * \param a A context smart-pointer.
 * \param b A multi-context-handle.
 * \return A smart pointer (handle) to the multi_context.
 */
inline multi_context_handle cons(boost::shared_ptr<context> a,
                                 multi_context_handle b) {
  return multi_context_handle(new multi_context(a, b));
}

/**
 * Return the first context of a multi_context.
 * \param x A handle to a multi_context.
 * \return The first context in the multi_context.
 */
inline context &car(multi_context_handle x) {
  return x->get();
}

/**
 * Return the "rest" of a multi_context. 
 * The rest is everything but the first context.
 * \param x A handle to a multi_context.
 * \return The "rest" of the multi_context.
 */
inline multi_context_handle cdr(multi_context_handle x) {
  return x->next();
}

}}}}

#endif
