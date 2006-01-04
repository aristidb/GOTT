// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Properties library
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

#ifndef GOTT_UTIL_PROPERTIES_PROPERTY_HPP
#define GOTT_UTIL_PROPERTIES_PROPERTY_HPP

#include <utility>
#include <sigc++/signal.h>

namespace gott {
namespace properties {

template<class Type> class property;
template<class Type> class read_property;
template<class Type> class write_property;

template<class Type> struct annotated {
  typedef std::pair<Type *, void *> pointer;
  typedef std::pair<Type const *, void *> const_pointer;
};

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
   * Set the value to another value.
   * \see set()
   */
  Type operator() (Type const &v) {
    return *read_write() = v;
  }

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
