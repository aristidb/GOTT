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
 * The Original Code is A Properties Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005
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

#ifndef GOTT_UTIL_PROPERTIES_PROPERTY_HPP
#define GOTT_UTIL_PROPERTIES_PROPERTY_HPP

#include <utility>
#include <sigc++/signal.h>

namespace gott {
namespace properties {

template<class Type> class property;
template<class Type> class read_property;
template<class Type> class write_property;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template<class Type> struct annotated {
  typedef std::pair<Type *, void *> pointer;
  typedef std::pair<Type const *, void *> const_pointer;
};
#endif

/**
 * A read-only reference to a property's value. Reference-counted.
 */
template<class Type>
class read_reference {
public:
  Type const *operator->() const {
    return ptr.first;
  }

  Type const &operator*() const {
    return *ptr.first;
  }
    
  read_reference(read_property<Type> const &p)
  : container(p), ptr(p.begin_read()), ref_count(new unsigned long(1)) {}

  read_reference(read_reference const &o)
    : container(o.container), ptr(o.ptr), ref_count(o.ref_count)
    { ++*ref_count; }

  ~read_reference() {
    if (--*ref_count == 0)
      container.end_read(ptr);
  }

  void operator=(read_reference const &o) {
    ~read_reference();
    new (this) read_reference(o);
  }

private:
  read_property<Type> const &container;
  typename annotated<Type>::const_pointer ptr;
  unsigned long *ref_count;
};

/**
 * A write-only reference to a property's value. Reference-counted.
 */
template<class Type>
class write_reference {
public:
  Type *operator->() {
    return ptr.first;
  }

  Type &operator*() {
    return *ptr.first;
  }
  
  write_reference(write_property<Type> &p)
  : container(p), ptr(p.begin_write()), ref_count(new unsigned long(1)) {}

  write_reference(write_reference const &o)
    : container(o.container), ptr(o.ptr), ref_count(o.ref_count)
    { ++*ref_count; }

  ~write_reference() {
    if (--*ref_count == 0)
      container.end_write(ptr);
  }

  void operator=(write_reference const &o) {
    ~write_reference();
    new (this) write_reference(o);
  }

private:
  write_property<Type> &container;
  typename annotated<Type>::pointer ptr;
  unsigned long *ref_count;
};

/**
 * A reference to a property's value. Reference-counted.
 */
template<class Type>
class read_write_reference {
public:
  Type *operator->() {
    return ptr.first;
  }

  Type &operator*() {
    return *ptr.first;
  }
    
  read_write_reference(property<Type> &p)
  : container(p), ptr(p.begin_read_write()), ref_count(new unsigned long(1)){}

  read_write_reference(read_write_reference const &o)
  : container(o.container), ptr(o.ptr), ref_count(o.ref_count)
  { ++*ref_count; }

  void operator=(read_write_reference const &o) {
    ~read_write_reference();
    new (this) read_write_reference(o);
  }

  ~read_write_reference() {
    if (--*ref_count == 0)
      container.end_read_write(ptr);
  }

private:
  property<Type> &container;
  typename annotated<Type>::pointer ptr;
  unsigned long *ref_count;
};

/**
 * Abstract read-only property base class.
 * \param Type The value's type.
 */
template<class Type>
class read_property {
public:
  virtual ~read_property() {}

  /**
   * Access the value for reading.
   * \return A read_reference<Type> to the value.
   */
  read_reference<Type> read() const { 
    return *this; 
  }

  /**
   * Get the value.
   * \return The value.
   */
  Type get() const {
    return *read();
  }

  /**
   * Get the value.
   * \see get()
   */
  Type operator()() const { return get(); }

  /**
   * Read the value to a function or functor.
   * \param func The function/functor to call.
   */
  template<class F>
  void apply_read(F func) const {
    func(*read());
  }

protected:
  typedef typename annotated<Type>::const_pointer annotated_const_pointer;

private:
  virtual annotated_const_pointer begin_read() const = 0;
  virtual void end_read(annotated_const_pointer) const = 0;
  friend class read_reference<Type>;
};

template<class Type>
class write_property {
public:
  virtual ~write_property() {}

  /**
   * Access the value for writing. Reading the value results in undefined
   * behaviour.
   * \return A write_reference<Type> to the value.
   */
  write_reference<Type> write() {
    return *this;
  }

  /**
   * Set the value to another value.
   * \param v The new value.
   */
  void set(Type const &v) {
    *write() = v;
  }

  /**
   * Set the value to another value.
   * \param v The new value.
   * \see set
   */
  void operator()(Type const &v) { set(v); }

  /**
   * Set the value to the result of a function or functor.
   * \param func The function/functor to call.
   */
  template<class F>
  void apply_write(F func) {
    *write() = func();
  }

protected:
  typedef typename annotated<Type>::pointer annotated_pointer;

private:
  virtual annotated_pointer begin_write() = 0;
  virtual void end_write(annotated_pointer) = 0;
  friend class write_reference<Type>;
};

/**
 * Abstract read & write property base class. 
 * \param Type The value's type.
 */
template<class Type>
class property : public read_property<Type>, public write_property<Type> {
public:
  /**
   * Access the value for changing.
   * \return A read_write_reference<Type> to the value.
   */
  read_write_reference<Type> read_write() {
    return *this; 
  }

  /**
   * Set the value to another value. Also, return the new value.
   */
  Type operator() (Type const &v) {
    return *read_write() = v;
  }

  using read_property<Type>::operator();

  /**
   * Change the value by applying a function or functor on it and resetting it
   * to the result.
   * \param func The function/functor to apply.
   */
  template<class F> 
  void apply_change(F func) {
    read_write_reference<Type> x(read_write());
    *x = func(*x);
  }

  /**
   * Change the value by applying a function or functor on a reference of it.
   * \param func The function/functor to apply.
   */
  template<class F>
  void apply_change_ref(F func) {
    func(*read_write());
  }

  /// Destructor.
  virtual ~property() {}

private:
  virtual typename annotated<Type>::pointer begin_read_write() = 0;
  virtual void end_read_write(typename annotated<Type>::pointer) = 0;
  friend class read_write_reference<Type>;
};

template<class T>
class notifying_property : public property<T> {
public:
  /**
   * Return a signal emitted whenever the value of the property changes.
   * \return A reference to the signal.
   */
  virtual sigc::signal0<void> &on_change() = 0;
};

}}

#endif
