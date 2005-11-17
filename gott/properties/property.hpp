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

/**
 * Abstract property base class. Properties somehow store a value which you can
 * set, read and change. They might emit a signal whenever their value is 
 * changed.
 * \param Type The value's type.
 */
template<class Type>
class property {
public:
  typedef Type value_type;
  typedef Type *pointer;
  typedef Type &reference;
  typedef Type const *const_pointer;
  typedef Type const &const_reference;

  typedef std::pair<pointer, void *> annotated_pointer;
  typedef std::pair<const_pointer, void *> annotated_const_pointer;

  /**
   * A read-only reference to a property's value. Reference-counted.
   */
  struct read_reference {
    const_pointer operator->() const {
      return ptr.first;
    }

    const_reference operator*() const {
      return *ptr.first;
    }
    
    read_reference(property const &p)
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

    property const &container;
    annotated_const_pointer ptr;
    unsigned long *ref_count;
  };

  /**
   * A write-only reference to a property's value. Reference-counted.
   */
  struct write_reference {
    pointer operator->() {
      return ptr.first;
    }

    reference operator*() {
      return *ptr.first;
    }
    
    write_reference(property &p)
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

    property &container;
    annotated_pointer ptr;
    unsigned long *ref_count;
  };

  /**
   * A reference to a property's value. Reference-counted.
   */
  struct read_write_reference {
    pointer operator->() {
      return ptr.first;
    }

    reference operator*() {
      return *ptr.first;
    }
    
    read_write_reference(property &p)
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

    property &container;
    annotated_pointer ptr;
    unsigned long *ref_count;
  };

  /**
   * Access the value for reading.
   * \return A read_reference to the value.
   */
  read_reference read() const { 
    return *this; 
  }

  /**
   * Access the value for changing.
   * \return A read_write_reference to the value.
   */
  read_write_reference read_write() {
    return *this; 
  }

  /**
   * Access the value for writing. Reading the value results in undefined
   * behaviour.
   * \return A write_reference to the value.
   */
  write_reference write() {
    return *this;
  }

  /**
   * Set the value to another value.
   * \param v The new value.
   */
  void set(const_reference v) {
    *write() = v;
  }

  /**
   * Get the value.
   * \return The value.
   */
  value_type get() const {
    return *read();
  }

  /**
   * Get the value.
   * \see get()
   */
  value_type operator()() const { return get(); }

  /**
   * Set the value to another value.
   * \see set()
   */
  const_reference operator() (value_type const &v) {
    return *read_write() = v;
  }

  /**
   * Change the value by applying a function or functor on it and resetting it
   * to the result.
   * \param func The function/functor to apply.
   */
  template<class F> 
  void apply_change(F func) {
    read_write_reference x(read_write());
    *x = func(*x);
  }

  /**
   * Read the value to a function or functor.
   * \param func The function/functor to call.
   */
  template<class F>
  void apply_read(F func) const {
    func(*read());
  }

  /**
   * Set the value to the result of a function or functor.
   * \param func The function/functor to call.
   */
  template<class F>
  void apply_write(F func) {
    *write() = func();
  }

  /// Destructor.
  virtual ~property() {}

protected:
  virtual annotated_const_pointer begin_read() const = 0;
  virtual annotated_pointer begin_write() = 0;
  virtual annotated_pointer begin_read_write() = 0;
  
  virtual void end_read(annotated_const_pointer) const = 0;
  virtual void end_write(annotated_pointer) = 0;
  virtual void end_read_write(annotated_pointer) = 0;
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
