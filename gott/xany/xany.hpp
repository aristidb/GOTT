// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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

#ifndef GOTT_XANY_INTERNAL
#error "Rather include <gott/xany.hpp>"
#else
#ifndef GOTT_XANY_XANY_HPP
#define GOTT_XANY_XANY_HPP

#include "spell.hpp"
#include "promote.hpp"
#include "operations_base.hpp"
#include "type_nr.hpp"
#include <gott/exceptions.hpp>
#include <boost/type_traits.hpp>
#include <cstddef>
#include <gott/visibility.hpp>

namespace gott { namespace xany {

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

  bool type_equals(Xany const &o) const {
    return spelled_type() == o.spelled_type();
  }

  /**
   * Checks for type compatibilty.
   */
  template<class T>
  bool compatible() const {
    std::type_info const &desire = spell<typename promote<T>::type>();
    std::type_info const &actual = spelled_type();
    return desire == actual;
  }

  /**
   * Empty this container and create a handle for creating a container with
   * the same value again, using recreate(). Leaks if not fed to recreate().
   */
  void *release() {
    void *result = place;
    place = 0;
    return result;
  }

  /**
   * Set to value stored in handle created with release(). The handle will be
   * invalid afterwards.
   */
  void recreate(void *p) {
    if (place)
      delete place;
    place = static_cast<placeholder *>(p);
  }

  /**
   * Get a numeric identifier (running number) of the current type.
   */
  long type_number() const {
    if (!place)
      return -1;
    return place->type_number();
  }

private:
  struct placeholder {
    virtual ~placeholder() {}
    virtual std::type_info const &type() const = 0;
    virtual std::type_info const &spelled_type() const = 0;
    virtual placeholder *clone() const = 0;
    virtual operations_base &get_operations() const = 0;
    virtual long type_number() const = 0;
  };

  template<class T>
  struct holder : public placeholder {
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
    long type_number() const {
      return type_nr<T>::value;
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
  if (!p.compatible<T>())
    throw gott::user_error("Xany: incompatible type");
  return promote<T>::get_back(static_cast<typename
   Xany::template holder<typename promote<T>::type>*>(p.place)->value);
}

/**
 * Get the data out of a typeless object for reading.
 * \param p The typeless object to get data out of.
 */
template<class T>
T Xany_cast(Xany const &p) {
  return Xany_cast_ref<typename boost::remove_reference<T>::type>(
      const_cast<Xany &>(p));
}

}}

#include "operations.hpp" // depends on Xany

#endif
#endif
