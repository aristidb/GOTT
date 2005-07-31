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

#include "rule_attr.hpp"
#include "../exceptions.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace structure { class repatcher; }

namespace schema {

class match;
class slotcfg;

/**
 * Rule-factory interface.
 * Classes implementing this can produce rule objects and are the standardized
 * way to do so.
 */
class GOTT_EXPORT rule_factory {
  public:   
  struct with_slotcfg;

  virtual bool accept_empty() const = 0;

  /**
   * Produce a rule object.
   * \param m The engine to work inside.
   */
  LOCAL virtual rule *get(match &m) const = 0;

  /**
   * Let the produced rule have a/another child.
   * Adds in the default location.
   * \param child The rule-factory producing the child.
   * \param repatcher The surrounding repatcher.
   */
  LOCAL virtual void add(rule_factory const &child) = 0;

  /**
   * Lets the produced rule have a child in a specific slot.
   * Slots semantics are optional.
   * \param child The rule-factory producing the child.
   * \param slot The slot to put the child into.
   * \param repatcher The surrounding repatcher.
   */
  LOCAL virtual void add(rule_factory const &child, unsigned slot);

  /**
   * Try to cast to a slotcfg-enabled rule-factory.
   */
  LOCAL with_slotcfg *get_with_slotcfg();

  static const unsigned no_slot = unsigned(-1);
    // signifies that a slot number is no slot
    // IMPORTANT: add(factory const &, unsigned) NEED NOT and
    //      SHALL NOT accept this slot-number: behaviour when
    //      passed this is undefined!

  virtual ~rule_factory() = 0;
};

/**
 * Some templates for providing the most accurate rule-factory builder.
 */
namespace factory_template {

typedef std::vector<rule_factory const *> container;

typedef
  rule_factory *(*rule_factory_builder)(rule_attr const &);

template<class T> struct enreg;

// Some factory implementation templates

/*
 * template <class T>
 * class X : public rule_factory {
 * public:
 *   X(rule_attr const &attr, [unsigned nochild]);
 *     // construct a factory that will produce with the given
 *    // attributes and the given number of children
 *
 *  static unsigned index();
 *    // return an index for this factory (parameter to get_factory)
 *
 *  static rule_factory *build(rule_attr const&attr, unsigned nochild);
 *    // rule_factory_builder: build an instance of this type with
 *    // given parameters
 *
 *  ... (implementation of rule_factory, details)
 * };
 */

template<class T> class nochild : public rule_factory {
public:
  nochild(rule_attr const &a) : attrib(a) {}

  bool accept_empty() const {
    return T::accept_empty();
  }

  void add(rule_factory const &child) { (void) child;
    throw dont_accept(L"children in nochild<>");
  }

  rule *get(match &m) const {
    return new T(attrib, m);
  }

  static unsigned index() {
    static enreg<nochild> e;
    return e.val;
  }

  static rule_factory *build(rule_attr const &a) {
    return new nochild(a);
  }

private:
  rule_attr attrib;
};

template<class T> class onechild : public rule_factory {
public:
  onechild(rule_attr const &a) : attrib(a) {}

  void add(rule_factory const &child) {
    sub = &child;
  }

  rule *get(match &m) const {
    return new T(*sub, attrib, m);
  }

  static unsigned index() {
    static enreg<onechild> e;
    return e.val;
  }

  static rule_factory *build(rule_attr const &a) {
    return new onechild(a);
  }

  bool accept_empty() const {
    return T::accept_empty(sub->accept_empty());
  }

private:
  rule_factory const *sub;
  rule_attr attrib;
};

template<class T, unsigned N> class somechildren : public rule_factory {
public:
  somechildren(rule_attr const &a) : pos(0), attrib(a) {}

  void add(rule_factory const &child) {
    sub[pos++] = &child;
  }

  void add(rule_factory const &child, unsigned slot) {
    sub[slot] = &child;
    pos = slot + 1;
  }

  rule *get(match &m) const {
    return new T(sub, attrib, m);
  }

  static unsigned index() {
    static enreg<somechildren> e;
    return e.val;
  }

  static rule_factory *build(rule_attr const &a) {
    return new somechildren(a);
  }

  bool accept_empty() const {
    return T::accept_empty(sub);
  }

private:
  rule_factory const *sub[N];
  unsigned pos;
  rule_attr attrib;
};

template<class T> class manychildren : public rule_factory {
public:
  manychildren(rule_attr const &a) : attrib(a) {}

  void add(rule_factory const &child) {
    sub.push_back(&child);
  }

  void add(rule_factory const &child, unsigned slot) {
    if (sub.size() <= slot)
      sub.resize(slot + 1);
    sub[slot] = &child;
  }

  rule *get(match &m) const {
    return new T(sub, attrib, m);
  }

  static unsigned index() {
    static enreg<manychildren> e;
    return e.val;
  }

  static rule_factory *build(rule_attr const &a) {
    return new manychildren(a);
  }

  bool accept_empty() const {
    return T::accept_empty(sub);
  }

private:
  container sub;
  rule_attr attrib;
};

// A rule_factory will want to enregister itself (add_factory) 
// and be assigned an index.
// This little class eases that.
template<class T> struct enreg {
  unsigned val;
  enreg() : val(add_factory(T::build)) {}
};

}

/**
 * Add a rule_factory type to the database and return its assigned index.
 * \param x The builder of the type to be added.
 * \return The type's index.
 */
unsigned add_factory(factory_template::rule_factory_builder x) GOTT_EXPORT;

/**
 * Get a rule_factory instance by its type's index.
 * \param x The type's index.
 * \param a The additional attributes.
 */
rule_factory *get_factory(unsigned x, rule_attr const &a);

}}}}

#endif
