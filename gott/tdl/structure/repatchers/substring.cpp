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

#include "substring.hpp"
#include "../repatcher_by_name.hpp"
#include <gott/tdl/exceptions.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <stdexcept>

using gott::tdl::structure::repatch_substring;
using gott::tdl::structure::writable_structure;

repatch_substring::repatch_substring(long l, long r) : left(l), right(r) {
  if (left < 0)
    throw std::invalid_argument(
        "repatch_substring: substring must begin at non-negative position");
}
repatch_substring::~repatch_substring() {}

writable_structure *repatch_substring::deferred_write(
    writable_structure &target) const {
  struct context : simple_repatcher_context {
    long left, right;
    context(writable_structure &target, long l, long r) 
      : simple_repatcher_context(target), left(l), right(r) {}
    void data(xany::Xany const &x) {
      if (!x.compatible<string>())
        throw failed_repatch("repatch_substring: need input string");
      string s = xany::Xany_cast<string>(x);
      long len = s.length();
      if (left > len || right > len || right < -len)
        throw failed_repatch(
            "repatch_substring: substring range out of bounds");
      string::utf32_range in = s.as_utf32(), out;
      if (right > 0) 
        out = offset(simply(in.begin()), left, right);
      else
        out = offset(in, left, right);
      if (out.end() < out.begin())
        throw failed_repatch("repatch_substring: invalid substring");
      target.data(xany::Xany(string(out)));
    }
  };
  return new context(target, left, right);
}

void repatch_substring::reg() {
  struct getter : public repatcher_getter {
    getter() : pos(outer) {}
    enum { outer, inner, p_left, p_right } pos;
    long left, right;
    boost::optional<long> delayed;
    void begin() {
      if (pos != outer) fail();
      pos = inner;
    }
    void end() {
      if (pos == outer || pos == inner)
        fail();
      pos = outer;
    }
    void data(xany::Xany const &x) {
      if (delayed)
        fail();
      switch (pos) {
      case p_left:
        left = xany::Xany_cast<long>(x);
        break;
      case p_right:
        right = xany::Xany_cast<long>(x);
        break;
      case inner:
        delayed = xany::Xany_cast<long>(x);
        break;
      case outer:
        fail();
      }
    }
    void add_tag(string const &s) {
      if (pos != inner)
        return;
      if (s == "left")
        pos = p_left;
      else if (s == "right")
        pos = p_right;
      else
        return;
      if (delayed) {
        if (pos == p_left)
          left = *delayed;
        else if (pos == p_right)
          right = *delayed;
        delayed = boost::none;
      }
    }
    void fail() {
      throw std::invalid_argument("repatch_substring arguments");
    }
    repatcher *result_alloc() const {
      return new repatch_substring(left, right);
    }
    static repatcher_getter *alloc() { return new getter; }
  };
  repatcher_by_name().add("substring", &getter::alloc);
}
