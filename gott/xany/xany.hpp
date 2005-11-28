// GOTT utilities
//
// Copyright (C) 2005 Aristid Breitkreuz
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) Xany later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef GOTT_TDL_UTIL_XANY_HPP
#define GOTT_TDL_UTIL_XANY_HPP

#include <gott/string/string.hpp>
#include <typeinfo>
#include <algorithm>
#include <boost/type_traits.hpp>
#include <cstddef>
#include <stdexcept>
#include <iosfwd>
#include <gott/xany/promote.hpp>

#include <gott/visibility.hpp>

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

/**
 * Typeless objects.
 * Has two special features:
 *   - Type promotion (see xany::promote).
 *   - Runtime-typed operations (see xany::operations_base).
 *
 * TODO: visibility-safety for free for all (optional)
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
   * Checks for type compatibilty.
   */
  template<class T>
  bool compatible() const {
    if (!place)
      return false;
    return typeid(typename promote<T>::type) == place->type();
  }

private:
  struct placeholder {
    virtual ~placeholder() {}
    virtual std::type_info const &type() const = 0;
    virtual placeholder *clone() const = 0;
    virtual operations_base &get_operations() const = 0;
  };

  template<class T> struct holder : public placeholder {
    holder(T const &v) : value(v) {}
    std::type_info const &type() const { 
      return typeid(T); 
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
  if (!v.empty())
    dynamic_cast<printable const &>(v.get_operations()).print(s, v);
  return s;
}

#define GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(T) \
  template<> struct operations<T> : equality_comparer<T>, printer<T> {}

GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(long);
GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(double);
GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(string);

}}

#endif
