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
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
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
#include <gott/range_algo.hpp>
#include <gott/string/string.hpp>

using gott::tdl::tdl_exception;

class tdl_exception::impl {
public:
  impl(string const &s) : len(s.size() + 1), msg(new char[len]) {
    copy(s.as_utf8().cast<char const *>(), msg);
    msg[s.size()] = '\0';
  }
  impl(impl const &o) : len(o.len), msg(new char[len]) {
    copy(range(o.msg, len), msg);
  }
  ~impl() throw() { delete [] msg; }
  std::size_t len;
  char *msg;
};

tdl_exception::tdl_exception(string const &msg)
: p(new impl(msg)) {}

tdl_exception::tdl_exception(tdl_exception const &o)
: std::exception(), p(new impl(*o.p)) {}

tdl_exception::~tdl_exception() throw() {}

char const *tdl_exception::what() const throw() {
  return p->msg;
}
