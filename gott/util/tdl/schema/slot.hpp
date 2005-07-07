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

#ifndef GOTT_UTIL_TDL_SCHEMA_SLOT_HPP
#define GOTT_UTIL_TDL_SCHEMA_SLOT_HPP

#include "rule.hpp"
#include "rule_factory.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace schema {

/**
 * "Repetition definition".
 * Specifies the number of children of a type a container may have.
 */
class slotcfg {
public:
  /// Simple modes (no parameter).
  enum simple_mode {
    /// Accept a single child
    one = 1,
    /// Optionally accept a single child
    optional = 2, 
    /// Accept at least one child
    some = 4,
    /// Accept any number of children
    list = 8
  };

  /// Sized modes (one \c std::size_t parameter).
  enum sized_mode {
    /// Accept minimally \arg x children
    minimum = 16,
    /// Accept maximally \arg x children
    maximum = 32,
    /// Accept exactly \arg x children
    exact = 64
  };

  /// Ranged modes (two \c std::size_t parameters).
  enum range_mode {
    /// Accept between \arg x and \arg y children. Borders inclusive.
    range = 128
  };

  /// Function modes (one \c callback parameter).
  enum function_mode {
    /// Accept as the specified by the function.
    function = 256
  };

  /** 
   * Classes of modes. Because modes are powers of two, they can be combined
   * into bitsets.
   */
  enum mode_classes {
    /// No possible mode.
    none = 0,
    /// All modes that accept maximally one child.
    single = one | optional, 
    /// All modes that accept more than one child.
    multiple = list | some | minimum | maximum | exact | range | function,
    /// All modes.
    all = single | multiple
  };

  /**
   * The combined mode type.
   * @see simple_mode
   * @see sized_mode
   * @see range_mode
   * @see function_mode
   */
  typedef unsigned mode;

  /**
   * The callback specifying whether to accept or not.
   * \param items The number of items.
   * \return See rule::expect.
   */
  typedef boost::function<rule::expect (std::size_t items)> callback;

  /**
   * Initialize a slotcfg object accepting one child.
   */
  slotcfg() EXPORT;

  /// Constructor.
  slotcfg(simple_mode) EXPORT;

  /// Constructor.
  slotcfg(sized_mode, std::size_t) EXPORT;

  /// Constructor.
  slotcfg(range_mode, std::size_t, std::size_t) EXPORT;

  /// Constructor.
  slotcfg(function_mode, callback const &) EXPORT;

  /// Constructor.
  slotcfg(slotcfg const &) EXPORT;

  /// Destructor.
  ~slotcfg() EXPORT;

  /// Whether the object accepts 0 children.
  bool prefix_optional() const;
  /// A variant of the object that does not accept 0 children.
  slotcfg no_optional() const;
  /// The mode the object is in.
  mode get_mode() const { return m; }

  /// Add an element.
  void add() EXPORT;
  /// Accept or not.
  rule::expect expectation() const EXPORT;

private:
  mode m;

  typedef
    boost::variant<
      std::size_t,
      std::pair<std::size_t, std::size_t>,
      callback
    >
  type_t;

  type_t type;
  std::size_t count;
};

/**
 * A rule-factory supporting slotcfg-objects. Accessed via 
 * rule::factory::get_with_slotcfg.
 */
struct EXPORT rule::factory::with_slotcfg {
  /**
   * Let the produced rule have a/another child.
   * Adds in the default location.
   * \param child The rule-factory producing the child.
   * \param cfg The "repetition definition" of the child.
   */
  virtual void add(factory const &child, slotcfg const &cfg) = 0;

  /**
   * Lets the produced rule have a child in a specific slot.
   * Slots semantics are optional. 
   * \param child The rule-factory producing the child.
   * \param slot The slot to put the child into.
   * \param cfg The "repetition definition" of the child.
   */
  virtual void add(factory const &child, unsigned slot, slotcfg const &cfg);

  virtual slotcfg::mode accepted_modes() const = 0;

  bool supports(slotcfg const &) const;

  virtual ~with_slotcfg() = 0;
};

namespace detail {
  template<slotcfg::mode A> struct factory_with_slotcfg 
  : public rule::factory, rule::factory::with_slotcfg {
    void add(factory const &) { 
      throw std::bad_cast(); 
    }

    slotcfg::mode accepted_modes() const { return A; }
  };
}

namespace factory_template {

template<class T, slotcfg::simple_mode Def, slotcfg::mode Accepted> 
struct slotcfg_onechild : public detail::factory_with_slotcfg<Accepted> {
public:
  slotcfg_onechild(rule::attributes const &a) : attrib(a) {}
  
  void add(rule::factory const &child) { 
    sub = &child;
    scfg = slotcfg(Def);
  }

  void add(rule::factory const &child, slotcfg const &cfg) { 
    sub = &child; 
    scfg = cfg;
  }
  
  rule *get(match &m) const {
    return new T(*sub, scfg, attrib, m);
  }

  static unsigned index() {
    static enreg<slotcfg_onechild> e;
    return e.val;
  }

  static rule::factory *build(rule::attributes const &a, unsigned) {
    return new slotcfg_onechild(a);
  }

private:
  rule::factory const *sub;
  slotcfg scfg;
  rule::attributes attrib;
};

template<class T, unsigned N, slotcfg::simple_mode Def, slotcfg::mode Accepted>
class slotcfg_somechildren : public detail::factory_with_slotcfg<Accepted> {
public:
  slotcfg_somechildren(rule::attributes const &a) : pos(0), attrib(a) {}

  void add(rule::factory const &child) { 
    add(child, slotcfg(Def));
  }

  void add(rule::factory const &child, unsigned slot) {
    add(child, slot, slotcfg(Def));
  }

  void add(rule::factory const &child, slotcfg const &cfg) {
    sub[pos] = child;
    scfg[pos] = cfg;
    ++pos;
  }

  void add(rule::factory const &child, unsigned slot, slotcfg const &cfg) {
    pos = slot;
    add(child, cfg);
  }

  rule *get(match &m) const {
    return new T(sub, scfg, attrib, m);
  }

  static unsigned index() {
    static enreg<slotcfg_somechildren> e; 
    return e.val;
  }

  static rule::factory *build(rule::attributes const &a, unsigned) {
    return new slotcfg_somechildren(a);
  }

private:
  rule::factory const *sub[N];
  unsigned pos;
  slotcfg scfg[N];
  rule::attributes attrib;
};

template<class T, slotcfg::simple_mode Def, slotcfg::mode Accepted>
class slotcfg_manychildren : public detail::factory_with_slotcfg<Accepted> {
public:
  typedef std::pair<rule::factory const *, slotcfg> element;
  
  slotcfg_manychildren(rule::attributes const &a, unsigned n) : attrib(a) {
    sub.reserve(n);
  }

  void add(rule::factory const &child) {
    add(child, slotcfg(Def));
  }

  void add(rule::factory const &child, unsigned slot) {
    add(child, slot, slotcfg(Def));
  }

  void add(rule::factory const &child, slotcfg const &cfg) {
    sub.push_back(element(&child, cfg));
  }

  void add(rule::factory const &child, unsigned slot, slotcfg const &cfg) {
    if (sub.size() <= slot)
      sub.resize(slot);
    sub[slot] = element(child, cfg);
  }

  rule *get(match &m) const {
    return new T(sub, attrib, m);
  }

  static unsigned index() {
    static enreg<slotcfg_manychildren> e;
    return e.val;
  }

  static rule::factory *build(rule::attributes const &a, unsigned n) {
    return new slotcfg_manychildren(a, n);
  }
  
private:
  std::vector<element> sub;
  rule::attributes attrib;
};

}

}}}}

#endif
