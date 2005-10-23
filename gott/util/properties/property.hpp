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

#ifndef GOTT_UTIL_PROPERTY_PROPERTY_HPP
#define GOTT_UTIL_PROPERTY_PROPERTY_HPP

#include "embedded_storage.hpp"
#include "trivial_aux.hpp"
#include "policy.hpp"
#include "translate.hpp"
#include <utility>

namespace gott {
namespace properties {

template<class T>
class property {
public:
  typedef T value_type;
  typedef T *pointer;
  typedef T &reference;
  typedef T const *const_pointer;
  typedef T const &const_reference;

  typedef std::pair<pointer, void *> annotated_pointer;
  typedef std::pair<const_pointer, void *> annotated_const_pointer;

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

    bool operator==(read_reference const &o) const {
      return ref_count == o.ref_count;
    }

    property const &container;
    annotated_const_pointer ptr;
    unsigned long *ref_count;
  };

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

    bool operator==(write_reference const &o) const {
      return ref_count == o.ref_count;
    }
    
    property &container;
    annotated_pointer ptr;
    unsigned long *ref_count;
  };

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

    ~read_write_reference() {
      if (--*ref_count == 0)
        container.end_read_write(ptr);
    }

    bool operator==(read_write_reference const &o) const {
      return ref_count == o.ref_count;
    }
    
    property &container;
    annotated_pointer ptr;
    unsigned long *ref_count;
  };

  read_reference read() const { 
    return *this; 
  }

  read_write_reference read_write() {
    return *this; 
  }

  write_reference write() {
    return *this;
  }

  void set(value_type const &v) {
    *write() = v;
  }

  const_reference get() const {
    return *read();
  }

  const_reference operator()() const { return get(); }

  const_reference operator() (value_type const &v) {
    return *read_write() = v;
  }

  template<class F> 
  void apply_change(F func) {
    read_write_reference x(read_write());
    *x = func(*x);
  }

  template<class F>
  void apply_read(F func) const {
    func(*read());
  }

  template<class F>
  void apply_write(F func) {
    *write() = func();
  }
  
  virtual ~property() {}

protected:
  virtual annotated_const_pointer begin_read() const = 0;
  virtual annotated_pointer begin_write() = 0;
  virtual annotated_pointer begin_read_write() = 0;
  
  virtual void end_read(annotated_const_pointer) const = 0;
  virtual void end_write(annotated_pointer) = 0;
  virtual void end_read_write(annotated_pointer) = 0;
};

template<
  class Type,
  class Notification = no_notification,
  class Storage = embedded_storage<Type>,
  class Lock = no_lock
>
class concrete_property :
  public property<Type>,
  public base<Notification>,
  public base<Storage>,
  public base<Lock>
{
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
  concrete_property(storage_p s = storage_policy(),
           notification_p n = notification_policy(),
           lock_p l = lock_policy())
  : storage(s), notifier(n), lock(l) {
  }

  concrete_property(value_type const &v,
           notification_p n = notification_policy(),
           lock_p l = lock_policy())
  : storage(v), notifier(n), lock(l) {
  }

private:
  annotated_const_pointer begin_read() const {
    read_lock::begin(lock);
    return annotated_const_pointer(storage.get_pointer(), 0);
  }

  annotated_pointer begin_write() {
    write_lock::begin(lock);
    return annotated_pointer(storage.get_pointer(), 0);
  }

  annotated_pointer begin_read_write() {
    read_write_lock::begin(lock);
    return annotated_pointer(storage.get_pointer(), 0);
  }

  void end_read(annotated_const_pointer p) const {
    storage.finish_pointer(p.first);
    read_lock::end(lock);
  }

  void end_write(annotated_pointer p) {
    storage.finish_pointer(p.first);
    write_lock::end(lock);
    notifier.notify(this);
  }
  
  void end_read_write(annotated_pointer p) {
    storage.finish_pointer(p.first);
    read_write_lock::end(lock);
    notifier.notify(this);
  }
  
  storage_s storage;
  notification_s notifier;
  mutable lock_s lock; //FIXME
};

template<
  class OldType, 
  class NewType = OldType, 
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
  translation_property(property<OldType> &b, translation_p t = translation_policy()) 
  : bound(b), translator(t) {}

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
