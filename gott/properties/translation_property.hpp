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

  annotated_pointer begin_read_write() {
    return translator.box(bound.read_write());
  }

  void end_read(annotated_const_pointer p) const {
    translator.template const_unbox<typename property<OldType>::read_reference>
      (p);
  }

  void end_write(annotated_pointer p) {
    translator.template unbox<typename property<OldType>::write_reference>(p);
  }
  
  void end_read_write(annotated_pointer p) {
    translator.template unbox<typename property<OldType>::read_write_reference>
      (p);
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
  annotated_pointer begin_read_write() { return delegate.begin_read_write(); }

  void end_read(annotated_const_pointer p) { delegate.end_read(p); }
  void end_write(annotated_pointer p) { delegate.end_write(p); }
  void end_read_write(annotated_pointer p) { delegate.end_read_write(p); }

  translation_property<OldType, NewType, Translation> delegate;
  sigc::signal0<void> on_change_signal;
};

}}

#endif
