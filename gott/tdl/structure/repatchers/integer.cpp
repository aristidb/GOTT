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
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#include "integer.hpp"
#include "../repatcher_by_name.hpp"
#include <gott/tdl/exceptions.hpp>
#include <cctype>

namespace structure = gott::tdl::structure;

using structure::repatch_integer;
using structure::writable_structure;

repatch_integer::repatch_integer() {}
repatch_integer::~repatch_integer() {}

writable_structure *
repatch_integer::deferred_write(writable_structure &s) const {
  struct context : simple_repatcher_context {
    context(writable_structure &s) : simple_repatcher_context(s) {}

    void data(xany::Xany const &x) {
      if (x.compatible<long>())
        target.data(x);
      else if (x.compatible<string>()) {
        string input = xany::Xany_cast<string>(x);
        long result;
        if (!is_integer(input, result))
          throw failed_repatch("repatch_integer: could not match integer");
        target.data(xany::Xany(result));
      } else
        throw failed_repatch("repatch_integer: could not match integer");
    }

    bool is_integer(string const &s, long &val) {
      long sign = 1;
      val = 0;
  
      string::utf8_range rng = s.as_utf8();
  
      if (rng.empty())
        return false;

      if (*rng.begin() == '-') {
        ++rng.begin();
        sign = -1;
        if (rng.empty())
          return false;
      }
  
      if (!std::isdigit(*rng.begin()))
        return false;
    
//    if (*it == L'0' && it[1] == L'x')
//      return is_hex(it + 2, s.end(), v, sign)

      for (; !rng.empty() && std::isdigit(*rng.begin()); ++rng.begin())
        val = val * 10 + (*rng.begin() - '0');

      val *= sign;
      return true;
    }
  };
  return new context(s);
}

void repatch_integer::reg() {
  struct getter : public repatcher_getter {
    getter() {}
    void begin(source_position const &) { fail(); }
    void end() { fail(); }
    void data(xany::Xany const &) { fail(); }
    void add_tag(string const &) { fail(); }
    void fail() {
      throw std::invalid_argument("repatch_integer arguments");
    }
    repatcher *result_alloc() const { return new repatch_integer(); }
    static repatcher_getter *alloc() { return new getter; }
  };
  repatcher_by_name().add("integer", &getter::alloc);
}
