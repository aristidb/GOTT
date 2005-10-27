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

#include "embedded_storage.hpp"
#include "external_storage.hpp"
#include "trivial_aux.hpp"
#include "policy.hpp"
#include "translate.hpp"
#include "sigc_notification.hpp"
#include <utility>

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
  const_reference get() const {
    return *read();
  }

  /**
   * Get the value.
   * \see get()
   */
  const_reference operator()() const { return get(); }

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

  /**
   * Thrown when on_change() is called but there is no notification signal
   * available.
   */
  class no_notification_error {};

  /**
   * Return a signal emitted whenever the value of the property changes.
   * \throw property::no_notification_error
   * \return A reference to the signal.
   */
  virtual sigc::signal0<void> &on_change() {
    throw no_notification_error();
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

/**
 * Generic property type. Allows for nearly arbitrary properties via policies.
 * Policies can be specified as reference types so they are stored by referenc
 * in the property.
 * Default: No notifications, embedded storage (if concrete_property is on the
 *   stack, the value will be there, too), no locking.
 * 
 * \param Type The type of the property's value.
 * \param Notification Notification policy for the property.
 * \param Storage Storage policy for the property.
 * \param Lock Locking policy for the property.
 */
template<
  class Type,
  class Notification = no_notification,
  class Storage = embedded_storage<Type>,
  class Lock = no_lock
>
class concrete_property : public property<Type> {
public:
  typedef typename property<Type>::value_type value_type;
  typedef typename property<Type>::const_pointer const_pointer;
  typedef typename property<Type>::pointer pointer;

  typedef typename policy<Storage>::class_type storage_policy;
  typedef typename policy<Lock>::class_type lock_policy;
  typedef typename policy<Notification>::class_type notification_policy;

private:
  typedef typename policy<Notification>::parameter notification_p;
  typedef typename policy<Notification>::storage notification_s;
  typedef typename policy<Storage>::parameter storage_p;
  typedef typename policy<Storage>::storage storage_s;
  typedef typename policy<Lock>::parameter lock_p;
  typedef typename policy<Lock>::storage lock_s;
  typedef typename lock_policy::read_lock read_lock;
  typedef typename lock_policy::write_lock write_lock;
  typedef typename lock_policy::read_write_lock read_write_lock;

  typedef typename property<Type>::annotated_pointer annotated_pointer;
  typedef typename property<Type>::annotated_const_pointer annotated_const_pointer;

public:
  /**
   * Constructor.
   * \param s [optional] The storage policy.
   * \param n [optional] The notification policy.
   * \param l [optional] The locking policy.
   */
  concrete_property(storage_p s = storage_policy(),
           notification_p n = notification_policy(),
           lock_p l = lock_policy())
  : storage(s), notifier(n), lock(l) {
  }

  /**
   * Constructor.
   * \param v The initial value.
   * \param n [optional] The notification policy.
   * \param l [optional] The locking policy.
   */
  concrete_property(value_type const &v,
           notification_p n = notification_policy(),
           lock_p l = lock_policy())
  : storage(v), notifier(n), lock(l) {
  }

  sigc::signal0<void> &on_change() {
    sigc::signal0<void> *p = notifier.get_on_change(this);
    if (!p)
      throw typename property<Type>::no_notification_error();
    return *p;
  }

private:
  annotated_const_pointer begin_read() const {
    return annotated_const_pointer(storage.get_pointer(), new read_lock(lock));
  }

  annotated_pointer begin_write() {
    return annotated_pointer(storage.get_pointer_noread(), new write_lock(lock));
  }

  annotated_pointer begin_read_write() {
    return annotated_pointer(storage.get_pointer(), new read_write_lock(lock));
  }

  void end_read(annotated_const_pointer p) const {
    storage.finish_pointer(p.first);
    delete static_cast<read_lock *>(p.second);
  }

  void end_write(annotated_pointer p) {
    storage.finish_pointer(p.first);
    delete static_cast<write_lock *>(p.second);
    notifier.notify(this);
  }
  
  void end_read_write(annotated_pointer p) {
    storage.finish_pointer(p.first);
    delete static_cast<write_lock *>(p.second);
    notifier.notify(this);
  }
  
  storage_s storage;
  notification_s notifier;
  lock_s lock;
};

/**
 * Translation property for re-typing / converting-in-spirit a property. Does 
 * not own the translated property, rather it keeps it by reference.
 * \param OldType The value type of the translated property.
 * \param NewType This property's value type.
 * \param Translation Policy for translating between real property and this.
 */
template<
  class OldType, 
  class NewType, 
  class Translation = 
    direct_translation<NewType, OldType>
> 
class translation_property : public property<NewType> {
public:
  typedef typename property<NewType>::value_type value_type;
  typedef typename property<NewType>::const_pointer const_pointer;
  typedef typename property<NewType>::pointer pointer;
  
  typedef typename policy<Translation>::class_type translation_policy;

private:
  typedef typename policy<Translation>::parameter translation_p;
  typedef typename policy<Translation>::storage translation_s;

  typedef typename property<NewType>::annotated_pointer annotated_pointer;
  typedef typename property<NewType>::annotated_const_pointer annotated_const_pointer;

public:
  /**
   * Constructor.
   * \param b The bound property.
   * \param t [optional] The translation policy.
   */
  translation_property(property<OldType> &b, translation_p t = translation_policy()) 
  : bound(b), translator(t) {}

  sigc::signal0<void> &on_change() {
    return bound.on_change();
  }

private:
  annotated_const_pointer begin_read() const {
    return translator.const_box(bound.read());
  }

  annotated_pointer begin_write() {
    return translator.box(bound.write());
  }

  annotated_pointer begin_read_write() {
    return translator.box(bound.read_write());
  }

  void end_read(annotated_const_pointer p) const {
    translator.template const_unbox<typename property<OldType>::read_reference>(p);
  }

  void end_write(annotated_pointer p) {
    translator.template unbox<typename property<OldType>::write_reference>(p);
  }
  
  void end_read_write(annotated_pointer p) {
    translator.template unbox<typename property<OldType>::read_write_reference>(p);
  }
  
  property<OldType> &bound;
  translation_s translator;
};

}}

#endif
