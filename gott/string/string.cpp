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
 * The Original Code is A String and Text Storage Library.
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

#include "string.hpp"
#include "iterator.hpp"
#ifndef NO_STDLIB
#include "stl.hpp"
#endif

#include <gott/range.hpp>
#include <gott/range_algo.hpp>
#include <gott/thunk.hpp>
#ifndef NO_STDLIB
#include <list>
#include <vector>
#include <ostream>
#include <string>
#endif

using gott::string;
using gott::range_t;

class string::representation {
public:
  representation(range_t<utf8_t const *> d, bool o = true)
  : size(d.size()), length(0), owned(o), data(d.begin()) {
  }

  enum foreign_tag { foreign_copy };

  representation(range_t<utf8_t const *> const &x, foreign_tag)
  : size(x.size()), length(0), owned(true),
      data(new utf8_t[size]) {
    utf8_t *out = const_cast<utf8_t *>(data);
    for (utf8_t const *it = x.begin(); it != x.end(); ++it)
      *out++ = *it;
  }

  ~representation() {
    if (owned) {
      delete [] data;
    }
  }

  std::size_t size;
  std::size_t length;
  bool owned;
  utf8_t const *data;
};

string::string(string const &o) : p(o.p) {}

string::~string() {}

void string::set_up(range_t<utf8_t const *> const &d, bool o) {
  p.reset(new representation(d, o));
}

void string::foreign(range_t<utf8_t const *> const &d) {
  p.reset(new representation(d, representation::foreign_copy));
}

range_t<gott::utf8_t const *> string::as_utf8() const {
  return range(p->data, p->size);
}

std::size_t string::length() const {
  if (!p->length)
    p->length = as_utf32().size();
  return p->length;
}

#ifndef NO_STDLIB
std::ostream &gott::operator<<(std::ostream &stream, string const &s) {
  for (utf8_t const *it = s.as_utf8().begin(); it < s.as_utf8().end(); ++it)
    stream << char(*it);
  return stream;
}

#ifdef HAVE_WIDE_STDLIB
std::wostream &gott::operator<<(std::wostream &stream, string const &s) {
  for (utf8_iterator it = s.as_utf32().begin(); it < s.as_utf32().end(); ++it)
    stream << wchar_t(*it);
  return stream;
}
#endif
#endif

bool gott::operator==(string const &a, string const &b) {
  if (a.as_utf8().begin() == b.as_utf8().begin())
    return true;
  return a.as_utf8() == b.as_utf8();
}

int gott::compare(string const &a, string const &b) {
  if (a.as_utf8().begin() == b.as_utf8().begin())
    return 0;

  utf8_t const *p1 = a.as_utf8().begin();
  utf8_t const *p2 = b.as_utf8().begin();
  for (; p1 != a.as_utf8().end() && p2 != b.as_utf8().end(); ++p1, ++p2)
    if (*p1 != *p2)
      break;
  if (p1 == a.as_utf8().end()) {
    if (p2 == b.as_utf8().end())
      return 0;
    return 1;
  } else if (p2 == b.as_utf8().end())
    return -1;
  
  return *p2 - *p1;
}

