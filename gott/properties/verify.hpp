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

#ifndef GOTT_UTIL_PROPERTIES_VERIFY_HPP
#define GOTT_UTIL_PROPERTIES_VERIFY_HPP

#include "property.hpp"
#include <sigc++/connection.h>
#include <boost/lambda/bind.hpp>

namespace gott {
namespace properties {

struct ignore_correct;
  
template<
  class Type,
  class Check, 
  class OnFailure, 
  class OnCorrect = ignore_correct
>
class verify {
public:
  verify(notifying_property<Type> &p, 
      Check c = Check(), 
      OnFailure f = OnFailure(),
      OnCorrect k = OnCorrect())
  : prop(p), check(c), failure(f), correct(k),
    change(prop.on_change().connect(
          boost::lambda::bind(&verify::action, this))) {}

  OnCorrect &on_correct() { return correct; }

  OnFailure &on_failure() { return failure; }

  ~verify() {
    change.disconnect();
  }

private:
  notifying_property<Type> &prop;
  Check check;
  OnFailure failure;
  OnCorrect correct;
  sigc::connection change;

  void action() {
    typename Check::context con = typename Check::context();
    if (!check(prop, con))
      failure(prop, con);
    else
      correct(prop);
  }
};

template<class T, class Check, class OnFailure, class PropertyType>
class owning_verify {
public:
  owning_verify(PropertyType const &p, 
      Check c = Check(), 
      OnFailure f = OnFailure())
  : prop(p), internal_verify(prop, c, f) {}

private:
  PropertyType prop;
  verify<T, Check, OnFailure> internal_verify;
};

template<class Type>
class check_range {
public:
  typedef Type context;

  check_range(Type const &s, Type const &b) : small(s), big(b) {}

  bool operator()(property<Type> &prop, context &con) {
    read_reference<Type> r = prop.read();
    if (*r < small) {
      con = small;
      return false;
    }
    if (*r > big) {
      con = big;
      return false;
    }
    return true;
  }

private:
  Type small;
  Type big;
};

template<class Type>
struct enforce_value {
  void operator()(property<Type> &prop, Type const &value) {
    prop.set(value);
  }
};

struct ignore_correct {
  template<class T>
  void operator()(T const &) {}
};

}}

#endif
