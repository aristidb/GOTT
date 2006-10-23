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
#include <gott/string/string.hpp>
#include <gott/thunk.hpp>

using tdl::tdl_error;
using gott::string;

class tdl_error::impl {
public:
  string module;
  string problem;
  source_position where;

  impl(string m, string p, source_position const &w)
    : module(m), problem(p), where(w) {}
};

namespace {
  gott::string make_string(string const &module, string const &problem) {
    string arr[] = { module, ": ", problem };
    return string(range(arr), string::concatenate);
  }

  gott::string make_string(string const &module, string const &problem,
      tdl::source_position const &p) {
    string arr[] = { 
      module, ": ", problem,
      (p.line > p.token_line || p.column > p.token_column + p.token.length() ?
       string(" after ") : string(" at ")), 
      p.file, 
      p.file != string() ? string(", ") : string(), 
      *gott::integer_to_string<gott::utf8_t>(p.token_line),
      ":", *gott::integer_to_string<gott::utf8_t>(p.token_column + 1),
      " (", p.token, ")"
    };
    return string(range(arr), string::concatenate);
  }
}

tdl_error::tdl_error(string const &module, string const &problem)
  : exception(make_string(module, problem)),
    p(new impl(module, problem, source_position())) {}

tdl_error::tdl_error(string const &module, string const &problem,
    source_position const &where)
  : exception(make_string(module, problem, where)),
    p(new impl(module, problem, where)) {}

tdl_error::tdl_error(tdl_error const &o)
  : gott::exception(*this), p(new impl(*o.p)) {}

tdl_error::~tdl_error() throw() {}

string const &tdl_error::module() const {
  return p->module;
}

string const &tdl_error::problem() const {
  return p->problem;
}

tdl::source_position const &tdl_error::where() const {
  return p->where;
}
