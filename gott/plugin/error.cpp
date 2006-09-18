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
 * The Original Code is A Plugin / Loadable Module Engine.
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

#include "error.hpp"
#include <gott/string/string.hpp>

using gott::plugin::failed_load;
using gott::string;

class failed_load::impl {
public:
  string kind;
  string which;
  string reason;

  impl(string const &k, string const &w, string const &r)
    : kind(k), which(w), reason(r) {}
};

failed_load::failed_load(
    string const &kind,
    string const &which,
    string const &reason) 
  : system_error("could not load " + kind + " " + which + ": " + reason),
    p(new impl(kind, which, reason)) {}
failed_load::~failed_load() throw() {}
failed_load::failed_load(failed_load const &o)
  : system_error(*this), p(new impl(*o.p)) {}

gott::string const &failed_load::kind() const { return p->kind; }
gott::string const &failed_load::which() const { return p->which; }
gott::string const &failed_load::reason() const { return p->reason; }
