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

#ifndef GOTT_UTIL_PROPERTIES_CONCRETE_PROPERTY_HPP
#define GOTT_UTIL_PROPERTIES_CONCRETE_PROPERTY_HPP

#include "property.hpp"
#include "policy.hpp"
#include "trivial_aux.hpp"
#include "embedded_storage.hpp"

namespace gott {
namespace properties {

template<class T, bool has_signal>
struct property_chooser { // true
  typedef notifying_property<T> type;
};

template<class T>
struct property_chooser<T, false> {
  typedef property<T> type;
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
class concrete_property 
: public
    property_chooser<Type, policy<Notification>::class_type::has_signal>::type {
public:
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
  typedef typename property<Type>::annotated_const_pointer 
    annotated_const_pointer;

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
  concrete_property(Type const &v,
           notification_p n = notification_policy(),
           lock_p l = lock_policy())
  : storage(v), notifier(n), lock(l) {
  }

  sigc::signal0<void> &on_change() {
    return notifier.get_on_change(this);
  }

private:
  annotated_const_pointer begin_read() const {
    return annotated_const_pointer(storage.get_pointer(), new read_lock(lock));
  }

  annotated_pointer begin_write() {
    return annotated_pointer(storage.get_pointer_noread(),new write_lock(lock));
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

}}

#endif
