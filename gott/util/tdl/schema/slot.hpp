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

class slotcfg {
public:
  enum simple_mode { 
    one = 1, optional = 2, some = 4, list = 8,
  };

  enum sized_mode {
    minimum = 16, maximum = 32, exact = 64,
  };

  enum range_mode {
    range = 128
  };

  enum function_mode {
    function = 256
  };

  enum mode_classes {
    none = 0,
    single = one | optional, 
    multiple = list | some | minimum | maximum | exact | range | function,
    all = single | multiple
  };

  typedef unsigned mode;
  typedef boost::function<rule::expect (std::size_t)> callback;

  slotcfg() EXPORT;
  slotcfg(simple_mode) EXPORT;
  slotcfg(sized_mode, std::size_t) EXPORT;
  slotcfg(range_mode, std::size_t, std::size_t) EXPORT;
  slotcfg(function_mode, callback const &) EXPORT;
  slotcfg(slotcfg const &) EXPORT;
  ~slotcfg() EXPORT;

  bool prefix_optional() const;
  slotcfg no_optional() const;
  mode get_mode() const { return m; }

  void add() EXPORT;
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

struct EXPORT rule::factory::with_slotcfg {
  virtual void add(factory const &child, slotcfg const &cfg) = 0;
  virtual void add(factory const &child, unsigned slot, slotcfg const &cfg);

  virtual slotcfg::mode accepted_modes() const = 0;
  bool supports(slotcfg const &) const;

  virtual ~with_slotcfg() = 0;
};

namespace detail {
  template<slotcfg::mode A> struct factory_with_slotcfg 
  : public rule::factory, rule::factory::with_slotcfg {
    void add(factory const &) { throw std::bad_cast(); }
    slotcfg::mode accepted_modes() const { return A; }
  };
}

namespace factory_template {

template<class T, slotcfg::simple_mode Def, slotcfg::mode Accepted> 
struct slotcfg_onechild : public detail::factory_with_slotcfg<Accepted> {
public:
  slotcfg_onechild(rule::attributes const &a) : attrib(a) {}
  
  void add(rule::factory const &rf) { sub = &rf; scfg = slotcfg(Def); }
  void add(rule::factory const &rf, slotcfg const &cfg) 
  { sub = &rf; scfg = cfg; }
  
  rule *get(match &m) const { return new T(*sub, scfg, attrib, m); }

  static unsigned index() { static enreg<slotcfg_onechild> e; return e.val; }
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
  void add(rule::factory const &f) { add(f, slotcfg(Def)); }
  void add(rule::factory const &f, unsigned i) { add(f, i, slotcfg(Def)); }

  void add(rule::factory const &f, slotcfg const &c) { 
    sub[pos] = f;
    scfg[pos] = c;
    ++pos;
  }

  void add(rule::factory const &f, unsigned i, slotcfg const &c) {
    pos = i;
    add(f, c);
  }

  rule *get(match &m) const { return new T(sub, scfg, attrib, m); }

  static unsigned index() { 
    static enreg<slotcfg_somechildren> e; return e.val; 
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

#if 0 // not done yet
template<class T, slotcfg::simple_mode Def, slotcfg::mode Accepted> 
class slotcfg_manychildren : public detail::factory_with_slotcfg<Accepted> {
public:
  slotcfg_manychildren(rule::attributes const &a, unsigned n) : attrib(a) { 
    sub.reserve(n); 
  }
 
  void add(rule::factory const &f) { sub.push_back(&f); }
  void add(rule::factory const &f, unsigned i) {
    if (sub.size() <= i)
      sub.resize(i + 1);
    sub[i] = &f;
  }
  rule *get(match &m) const { return new T(sub, attrib, m); }

  static unsigned index() { 
    static enreg<slotcfg_manychildren> e; return e.val; 
  }
  static rule::factory *build(rule::attributes const &a, unsigned n) { 
    return new slotcfg_manychildren(a, n); 
  }
  
private:
  container sub;
  rule::attributes attrib;
};
#endif

}

}}}}

#endif
