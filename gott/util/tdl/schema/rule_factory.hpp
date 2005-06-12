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

#ifndef GOTT_UTIL_TDL_SCHEMA_RULE_FACTORY_HPP
#define GOTT_UTIL_TDL_SCHEMA_RULE_FACTORY_HPP

#include "rule.hpp"
#include "rule_attr.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

class match;
class slotcfg;

/**
 * Rule-factory interface.
 * Classes implementing this can produce rule objects and are the standardized
 * way to do so.
 */
struct EXPORT rule::factory {
  struct with_slotcfg;

  /**
   * Produce a rule object.
   * \param m The engine to work inside.
   */
  virtual rule *get(match &m) const = 0;

  /**
   * Let the produced rule have a/another child.
   * Adds in the default location.
   * \param child The rule-factory producing the child.
   */
  virtual void add(factory const &child) = 0;

  /**
   * Lets the produced rule have a child in a specific slot.
   * Slots semantics are optional.
   * \param child The rule-factory producing the child.
   * \param slot The slot to put the child into.
   */
  virtual void add(factory const &child, unsigned slot);

  /**
   * Try to cast to a slotcfg-enabled rule-factory.
   */
  with_slotcfg *get_with_slotcfg();

  static const unsigned no_slot = unsigned(-1);
    // signifies that a slot number is no slot
    // IMPORTANT: add(factory const &, unsigned) NEED NOT and
    //      SHALL NOT accept this slot-number: behaviour when
    //      passed this is undefined!

  virtual ~factory() = 0;
};

/**
 * Some templates for providing the most accurate rule-factory builder.
 */
namespace factory_template {

typedef std::vector<rule::factory const *> container;

typedef
  rule::factory *(*rule_factory_builder)(rule::attributes const &, unsigned);

template<class T> struct enreg;

// Some factory implementation templates

/*
 * template <class T>
 * class X : public rule::factory {
 * public:
 *   X(rule::attributes const &attr, [unsigned nochild]);
 *     // construct a factory that will produce with the given
 *    // attributes and the given number of children
 *
 *  static unsigned index();
 *    // return an index for this factory (parameter to get_factory)
 *
 *  static rule::factory *build(rule::attributes const&attr, unsigned nochild);
 *    // rule_factory_builder: build an instance of this type with
 *    // given parameters
 *
 *  ... (implementation of rule::factory, details)
 * };
 */

template<class T> class nochild : public rule::factory {
public:
  nochild(rule::attributes const &a) : attrib(a) {}

  void add(rule::factory const &) {
    throw ::std::bad_exception();
  }

  rule *get(match &m) const {
    return new T(attrib, m);
  }

  static unsigned index() {
    static enreg<nochild> e;
    return e.val;
  }

  static rule::factory *build(rule::attributes const &a, unsigned) {
    return new nochild(a);
  }

private:
  rule::attributes attrib;
};

template<class T> class onechild : public rule::factory {
public:
  onechild(rule::attributes const &a) : attrib(a) {}

  void add(rule::factory const &f) {
    sub = &f;
  }

  rule *get(match &m) const {
    return new T(*sub, attrib, m);
  }

  static unsigned index() {
    static enreg<onechild> e;
    return e.val;
  }

  static rule::factory *build(rule::attributes const &a, unsigned) {
    return new onechild(a);
  }

private:
  rule::factory const *sub;
  rule::attributes attrib;
};

template<class T, unsigned N> class somechildren : public rule::factory {
public:
  somechildren(rule::attributes const &a) : pos(0), attrib(a) {}

  void add(rule::factory const &f) {
    sub[pos++] = &f;
  }

  void add(rule::factory const &f, unsigned i) {
    sub[i] = &f;
    pos = i + 1;
  }

  rule *get(match &m) const {
    return new T(sub, attrib, m);
  }

  static unsigned index() {
    static enreg<somechildren> e;
    return e.val;
  }

  static rule::factory *build(rule::attributes const &a, unsigned) {
    return new somechildren(a);
  }

private:
  rule::factory const *sub[N];
  unsigned pos;
  rule::attributes attrib;
};

template<class T> class manychildren : public rule::factory {
public:
  manychildren(rule::attributes const &a, unsigned n) : attrib(a) {
    sub.reserve(n);
  }

  void add(rule::factory const &f) {
    sub.push_back(&f);
  }

  void add(rule::factory const &f, unsigned i) {
    if (sub.size() <= i)
      sub.resize(i + 1);
    sub[i] = &f;
  }

  rule *get(match &m) const {
    return new T(sub, attrib, m);
  }

  static unsigned index() {
    static enreg<manychildren> e;
    return e.val;
  }

  static rule::factory *build(rule::attributes const &a, unsigned n) {
    return new manychildren(a, n);
  }

private:
  container sub;
  rule::attributes attrib;
};

// A rule::factory will want to enregister itself (add_factory) 
// and be assigned an index.
// This little class eases that.
template<class T> struct enreg {
  unsigned val;
  enreg() : val(add_factory(T::build)) {}
};

}

/**
 * Add a rule::factory type to the database and return its assigned index.
 * \param x The builder of the type to be added.
 * \return The type's index.
 */
unsigned add_factory(factory_template::rule_factory_builder x) EXPORT;

/**
 * Get a rule::factory instance by its type's index.
 * \param x The type's index.
 * \param a The additional attributes.
 * \param length The expected number of children (optional depending on type).
 */
rule::factory *get_factory(unsigned x, rule::attributes const &a,
                           unsigned length);

}}}}

#endif
