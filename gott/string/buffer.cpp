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

#include "buffer.hpp"
#include "convert.hpp"
#include "iterator.hpp"
#include "string.hpp"
#ifndef NO_STDLIB
#include <algorithm>
#include <gott/range_algo.hpp>
#endif

using gott::string_buffer;
using gott::utf32_t;

class string_buffer::representation {
public:
  representation() : begin(0), end(0), storage_end(0) {}

  ~representation() { delete [] begin; }
  
  utf32_t *begin, *end, *storage_end;

  void ensure(std::size_t add_len) {
    std::size_t old_len = end - begin;
    if (std::size_t(storage_end - end) < add_len)
      grow(old_len + add_len);
    end = begin + old_len + add_len;
  }

private:
  void grow(std::size_t length) {
    std::size_t new_size = 1;
    while (new_size < length) 
      new_size <<= 1;
    utf32_t *old = begin;
    begin = new utf32_t[new_size];
    utf32_t *out = begin;
    for (utf32_t *pos = old; pos != end; ++pos)
      *out++ = *pos;
    delete [] old;
    storage_end = begin + new_size;
  }
};

string_buffer::string_buffer()
: data(new representation) {}

string_buffer::~string_buffer() { delete data; }

utf32_t const *string_buffer::begin() const {
  return data->begin;
}

utf32_t const *string_buffer::end() const {
  return data->end;
}

std::size_t string_buffer::size() const {
  return data->end - data->begin;
}

string_buffer::iterator string_buffer::erase(range_t<iterator> const &r) {
  iterator a = r.begin();
  iterator b = r.end();

  while (b != data->end)
    *a++ = *b++;
  data->end = a;
  return r.begin();
}

gott::range_t<string_buffer::iterator>
string_buffer::insert(iterator p, std::size_t len) {
  if (len != 0) {
    if (std::size_t(data->storage_end - data->end) < len) {
      std::size_t pp = p - data->begin;
      data->ensure(len);
      p = data->begin + pp;
    } else
      data->end += len;
    for (iterator it = data->end - 1; it >= p + len; --it)
      *it = *(it - len);
  }
  return range(p, len);
}

void string_buffer::operator+=(string const &s) {
  utf8_iterator in = s.as_utf32().begin();
  for (iterator pos = append(s.length()).begin(); pos != end(); ++pos)
    *pos = *in++;
}

void string_buffer::operator+=(string_buffer const &sb) {
  range_t<const_iterator> r = range(sb);
  insert(end(), r.begin(), r.end());
}
