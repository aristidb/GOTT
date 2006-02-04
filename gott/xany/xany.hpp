/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A Type-agnostic Storage Class.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef GOTT_XANY_XANY_HPP
#define GOTT_XANY_XANY_HPP

#include "promote.hpp"
#include <gott/string/string.hpp>
#include <typeinfo>
#include <algorithm>
#include <boost/type_traits.hpp>
#include <cstddef>
#include <stdexcept>
#include <iosfwd>

#include <gott/visibility.hpp>

#include <iostream>

namespace gott {
namespace xany {

/**
 * Dummy basetype for "typeless operations".
 *
 * Example usage:
 * @code
 * Xany v(4);
 * std::cout << v;
 * @endcode
 * Which is behind the scenes:
 * @code
 * dynamic_cast<printable&>(v.get_operations()).print(std::cout, v);
 * @endcode
 */
struct operations_base {
  GOTT_EXPORT virtual ~operations_base() = 0;
};

template<class T> struct operations;

template<> struct operations<void> : operations_base {};

template<class T> struct GOTT_EXPORT spelled_t {};

template<class T>
inline std::type_info const &spell() {
  return typeid(spelled_t<T>);
}

/**
 * Typeless objects.
 * Has two special features:
 *   - Type promotion (see xany::promote).
 *   - Runtime-typed operations (see xany::operations_base).
 */
class Xany {
  template<class T> class holder;
public:
  /// Construct an empty typeless object.
  Xany() : place(0) {}

  /**
   * Constructs a typeless object from data.
   * \param v The data to save.
   */
  template<class T> explicit Xany(T v)
    : place(new holder<typename promote<T>::type>(promote<T>::get(v))) {}

  ~Xany() { 
    delete place; 
  }

  /// Copy constructor.
  Xany(Xany const &o) : place(o.place ? o.place->clone() : 0) {}

  /**
   * Swaps the object's contents with another object.
   * \param o The object to swap with.
   */
  Xany &swap(Xany &o) { 
    std::swap(place, o.place); 
    return *this; 
  }

  /**
   * (Over-)Write another object's data to the object.
   * \param o The object to copy from.
   */
  Xany &operator=(Xany const &o) { 
    Xany(o).swap(*this); 
    return *this; 
  }

  /**
   * (Over-)Write data to the object.
   * \param o The data to write.
   */
  template<class T>
  Xany &operator=(T const &o) {
    Xany(o).swap(*this); 
    return *this; 
  }

  /**
   * Gets the operations supported by the object's data.
   * Cast to the actually needed operations.
   * \return A basetype reference to the operations.
   */
  operations_base const &get_operations() const { 
    if (!place) {
      static operations<void> nothing;
      return nothing;
    }
    return place->get_operations();
  }

  /**
   * Checks whether the object is empty.
   */
  bool empty() const { 
    return !place; 
  }

  /**
   * Returns the object's data type.
   */
  std::type_info const &type() const {
    return place ? place->type() : typeid(void);
  }

  /**
   * Returns the object's spelled type.
   */
  std::type_info const &spelled_type() const {
    return place ? place->spelled_type() : spell<void>();
  }

  /**
   * Checks for type compatibilty.
   */
  template<class T>
  bool compatible() const {
    if (!place)
      return false;
#ifdef BUG_FIXED_GCC_TEMPLATE_TYPEINFO
    std::type_info const &desire = spell<typename promote<T>::type>();
    std::type_info const &actual = place->spelled_type();
#else
    std::type_info const &desire = typeid(typename promote<T>::type);
    std::type_info const &actual = place->type();
#endif
    return desire == actual;
  }

private:
  struct placeholder {
    virtual ~placeholder() {}
    virtual std::type_info const &type() const = 0;
    virtual std::type_info const &spelled_type() const = 0;
    virtual placeholder *clone() const = 0;
    virtual operations_base &get_operations() const = 0;
  };

  template<class T> struct holder : public placeholder {
    holder(T const &v) : value(v) {}
    std::type_info const &type() const { 
      return typeid(T); 
    }
    std::type_info const &spelled_type() const {
      return spell<T>();
    }
    placeholder *clone() const { 
      return new holder(value); 
    }
    operations_base &get_operations() const {
      static operations<T> o;
      return o;
    }

    T value;
  };

  template<class T> 
  friend typename promote<T>::reference Xany_cast_ref(Xany &);

  placeholder *place;
};

/**
 * Get the data out of a typeless object for modification.
 * \param p The typeless object to get data out of.
 */
template<class T>
typename promote<T>::reference
Xany_cast_ref(Xany &p) {
  if (p.empty() || p.type() != typeid(typename promote<T>::type))
    throw std::bad_cast();
  return promote<T>::get_back(static_cast<typename
   Xany::template holder<typename promote<T>::type>*>(p.place)->value);
}

/**
 * Get the data out of a typeless object for reading.
 * \param p The typeless object to get data out of.
 */
template<class T>
T Xany_cast(Xany const &p) {
  return Xany_cast_ref<T>(const_cast<Xany &>(p));
}

/**
 * Any equality-comparable Xany-compatible type has this interface in its
 * operations.
 */
struct GOTT_EXPORT equality_comparable : virtual operations_base {
  virtual bool equals(Xany const &, Xany const &) const = 0;
  virtual ~equality_comparable() = 0;
};

template<class T> struct equality_comparer : equality_comparable {
  bool equals(Xany const &a, Xany const &b) const {
    return Xany_cast<T>(a) == Xany_cast<T>(b);
  }
};

/**
 * The default operations implementation.
 */
template<class T> 
struct operations : equality_comparer<T> {};

/**
 * Checks whether two typeless objects are equal.
 */
GOTT_EXPORT bool operator==(Xany const &lhs, Xany const &rhs);

/**
 * Checks whether two typeless objects differ.
 */
GOTT_EXPORT bool operator!=(Xany const &lhs, Xany const &rhs);

/**
 * Any printable Xany-compatible type has this interface in its operations.
 */
struct GOTT_EXPORT printable : virtual operations_base {
  virtual void print(std::ostream &, Xany const &) const = 0;

#ifdef HAVE_WIDE_STDLIB
  virtual void print(std::wostream &, Xany const &) const = 0;
#endif

  virtual ~printable() = 0;
};

template<class T> struct printer : printable {
  void print(std::ostream &s, Xany const &v) const {
    s << Xany_cast<T>(v);
  }

#ifdef HAVE_WIDE_STDLIB
  void print(std::wostream &s, Xany const &v) const {
    s << Xany_cast<T>(v);
  }
#endif
};

/**
 * Print a typeless object to a stream.
 * \param s The stream to write to.
 * \param v The object to print.
 * \return A reference to s.
 */
template<class Ch, class ChT>
std::basic_ostream<Ch, ChT> &
operator<<(std::basic_ostream<Ch, ChT> &s, Xany const &v) {
  if (!v.empty()) {
    printable const &printer = 
      dynamic_cast<printable const &>(v.get_operations());
    printer.print(s, v);
  }
  return s;
}

#define GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(T) \
  template<> struct operations<T> : equality_comparer<T>, printer<T> {}

GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(long);
GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(double);
GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(string);

}}

#endif
