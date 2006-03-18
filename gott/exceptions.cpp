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
 * The Original Code is Basic Exception Classes for GOTT.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include "exceptions.hpp"
#include <gott/string/string.hpp>
#include <gott/range_algo.hpp>

using gott::string;

gott::exception::exception(string const &desc) throw() 
  : data(desc.c_string_alloc()) {}
gott::exception::~exception() throw() { delete [] data; }
char const *gott::exception::what() const throw() { return data; }

gott::internal_error::internal_error(string const &desc) throw()
  : exception(desc) {}
gott::internal_error::~internal_error() throw() {}

gott::user_error::user_error(string const &desc) throw()
  : exception(desc) {}
gott::user_error::~user_error() throw() {}

gott::system_error::system_error(string const &desc) throw()
  : exception(desc) {}
gott::system_error::~system_error() throw() {}
