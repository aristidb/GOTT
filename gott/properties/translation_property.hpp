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

#ifndef GOTT_UTIL_PROPERTIES_TRANSLATION_PROPERTY_HPP
#define GOTT_UTIL_PROPERTIES_TRANSLATION_PROPERTY_HPP

#include "property.hpp"
#include "translate.hpp"

namespace gott {
namespace properties {

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
  class Translation = direct_translation<NewType, OldType>
> 
class translation_property : public property<NewType> {
public:
  typedef typename policy<Translation>::class_type translation_policy;

private:
  typedef typename policy<Translation>::parameter translation_p;
  typedef typename policy<Translation>::storage translation_s;

  typedef typename property<NewType>::annotated_pointer annotated_pointer;
  typedef typename property<NewType>::annotated_const_pointer 
    annotated_const_pointer;

public:
  /**
   * Constructor.
   * \param b The bound property.
   * \param t [optional] The translation policy.
   */
  translation_property(property<OldType> &b, 
      translation_p t = translation_policy()) 
  : bound(b), translator(t) {}

private:
  annotated_const_pointer begin_read() const {
    return translator.const_box(bound.read());
  }

  annotated_pointer begin_write() {
    return translator.box(bound.write());
  }

  annotated_pointer begin_change() {
    return translator.box(bound.change());
  }

  void end_read(annotated_const_pointer p) const {
    translator.template const_unbox<read_reference<OldType> >(p);
  }

  void end_write(annotated_pointer p) {
    translator.template unbox<write_reference<OldType> >(p);
  }
  
  void end_change(annotated_pointer p) {
    translator.template unbox<change_reference<OldType> >(p);
  }
  
  property<OldType> &bound;
  translation_s translator;
};

template<
  class OldType,
  class NewType,
  class Translation = direct_translation<NewType, OldType>
> 
class notifying_translation_property : public notifying_property<NewType> {
public:
  typedef typename property<NewType>::value_type value_type;
  typedef typename property<NewType>::const_pointer const_pointer;
  typedef typename property<NewType>::pointer pointer;
  
  typedef typename policy<Translation>::class_type translation_policy;

private:
  typedef typename policy<Translation>::parameter translation_p;
  typedef typename policy<Translation>::storage translation_s;

  typedef typename property<NewType>::annotated_pointer annotated_pointer;
  typedef typename property<NewType>::annotated_const_pointer 
    annotated_const_pointer;

public:
  /**
   * Constructor.
   * \param b The bound property.
   * \param t [optional] The translation policy.
   */
  notifying_translation_property(notifying_property<OldType> &b, 
      translation_p t = translation_policy()) 
  : delegate(b, t), on_change_signal(b.on_change()) {}

  sigc::signal0<void> on_change() { return on_change_signal; }

private:
  annotated_const_pointer begin_read() { return delegate.begin_read(); }
  annotated_pointer begin_write() { return delegate.begin_write(); }
  annotated_pointer begin_change() { return delegate.begin_change(); }

  void end_read(annotated_const_pointer p) { delegate.end_read(p); }
  void end_write(annotated_pointer p) { delegate.end_write(p); }
  void end_change(annotated_pointer p) { delegate.end_change(p); }

  translation_property<OldType, NewType, Translation> delegate;
  sigc::signal0<void> &on_change_signal;
};

}}

#endif
