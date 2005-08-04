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

#include <gott/util/misc/commonheaders.hpp>
#include <gott/util/properties/embedded_storage.hpp>
#include <gott/util/properties/trivial_aux.hpp>
#include <gott/util/properties/policy.hpp>

namespace gott {
namespace properties {

template<
  class Type,
  class Notification = no_notification,
  class Storage = embedded_storage<Type>,
  class Access = direct_access<Type, Storage>,
  class Lock = no_lock
>
class property {
public:
  typedef Type value_type;
  typedef typename policy<Storage>::class_type storage_policy;
  typedef typename policy<Lock>::class_type lock_policy;
  typedef typename policy<Access>::class_type access_policy;
  typedef typename policy<Notification>::class_type notification_policy;

private:
  typedef typename policy<Notification>::parameter notification_p;
  typedef typename policy<Notification>::storage notification_s;
  typedef typename policy<Storage>::parameter storage_p;
  typedef typename policy<Storage>::storage storage_s;
  typedef typename policy<Access>::parameter access_p;
  typedef typename policy<Access>::storage access_s;
  typedef typename policy<Lock>::parameter lock_p;
  typedef typename policy<Lock>::storage lock_s;
  typedef typename lock_policy::read_lock read_lock;
  typedef typename lock_policy::write_lock write_lock;
  typedef typename lock_policy::read_write_lock read_write_lock;

public:
  property(storage_p s = storage_policy(),
           notification_p n = notification_policy(),
           access_p a = access_policy(),
           lock_p l = lock_policy())
  : storage(s), access(a), notifier(n), lock(l) {
  }

  property(storage_p s,
           access_p a,
           notification_p n = notification_policy(),
           lock_p l = lock_policy())
  : storage(s), access(a), notifier(n), lock(l) {
  }

  property(value_type const &v,
           notification_p n = notification_policy(),
           access_p a = access_policy(),
           lock_p l = lock_policy())
  : storage(v), access(a), notifier(n), lock(l) {
  }

  void set(value_type const &v) {
    write_lock w(lock);
    if (access.set(storage, v))
      notifier.notify(this);
  }

  typename storage_policy::const_reference get() const {
    read_lock r(lock);
    return access.get(storage);
  }

  typename storage_policy::const_reference operator()() const { return get(); }

  typename storage_policy::const_reference operator() (value_type const &v) {
    read_write_lock rw(lock);
    if (access.set(storage, v))
      notifier.notify(this);
    return access.get(storage);
  }

  template<class T> 
  void apply_change(T func) {
    read_write_lock rw(lock);
    value_type old = access.get(storage);
    if (access.set(storage, func(old)))
      notifier.notify(this);
  }

  template<class T>
  void apply_read(T func) const {
    read_lock r(lock);
    func(access.get(storage));
  }

  template<class T>
  void apply_write(T func) {
    write_lock w(lock);
    if (access.set(storage, func()))
      notifier.notify(this);
  }

  struct read_reference {
    typename storage_policy::const_pointer operator->() const {
      return ptr;
    }

    typename storage_policy::const_reference operator*() const {
      return *ptr;
    }
    
    read_reference(property const &p)
      : container(p), ptr(p.access.build(p.storage)), lock(p.lock) {}

    ~read_reference() {
      container.access.done_with(ptr);
    }

    property const &container;
    typename storage_policy::const_pointer ptr;
    read_lock lock;
  };

  template<class XWLock>
  struct x_write_reference {
    typename storage_policy::pointer operator->() {
      return ptr;
    }

    typename storage_policy::reference operator*() {
      return *ptr;
    }
    
    x_write_reference(property &p)
      : container(p), ptr(p.access.build(p.storage)), lock(p.lock) {}

    ~x_write_reference() {
      if (container.access.done_with(ptr))
        container.notifier.notify(&container);
    }

    property &container;
    typename storage_policy::pointer ptr;
    XWLock lock;
  };


  template<class> friend class reference_implementation;

  read_reference read() const { 
    return *this; 
  }

  x_write_reference<read_write_lock> read_write() {
    return *this; 
  }

  x_write_reference<write_lock> write() {
    return *this;
  }

  notification_policy const &notification() const {
    return notifier;
  }

private:
  storage_s storage;
  access_s access;
  notification_s notifier;
  lock_s lock;
};

}}

#endif
