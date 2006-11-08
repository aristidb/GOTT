// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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

#ifndef GOTT_UTIL_PROPERTIES_EXTERNAL_STORAGE_HPP
#define GOTT_UTIL_PROPERTIES_EXTERNAL_STORAGE_HPP

#include <boost/function.hpp>

namespace gott { namespace properties {

/**
 * External storage policy. Gets and sets its function via a function/functor.
 * \param T The values' type.
 * \param Input The input functor type.
 * \param Output The output functor type.
 */
template<
  class T, 
  class Input = boost::function<T ()>, 
  class Output = boost::function<void (T)>
>
class external_storage { // (optional) TODO: use thread-local-storage
public:
  /**
   * Constructor.
   * \param in_f The function/functor for getting the value.
   * \param out_f The function/functor for setting the value.
   */
  external_storage(Input in_f = Input(), Output out_f = Output())
  : in(in_f), out(out_f) {}

  T *get_pointer() { return new T(in()); }
  T *get_pointer_noread() { return new T; }
  T const *get_pointer() const { return new T(in()); }
  void finish_pointer(T *p) { out(*p); delete p; }
  void finish_pointer(T const *p) const { delete p; }
  
private:
  Input in;
  Output out;
};

}}
#endif
