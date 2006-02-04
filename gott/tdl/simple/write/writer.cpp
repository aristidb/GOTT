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

#include "writer.hpp"
#include <ostream>

using std::ostream;
using std::string;

namespace gott {
namespace tdl {
namespace simple {
  
class writer::impl {
public:
  impl(ostream &os, unsigned i)
  : stream(os), level(0), indentation_width(i), newline(false), 
    last_block(false) {}

  ostream &stream;
  unsigned level;
  unsigned indentation_width;
  bool newline;
  bool last_block;
  bool level_change;

  void indent();
  void print_as_block(string const &s);
};

writer::writer(ostream &os, unsigned i) : p(new impl(os, i)) {}

void writer::impl::indent() {
  for (unsigned i = 2; i < level; ++i)
    for (unsigned j = 0; j < indentation_width / 2; ++j)
      stream << ' ';

  if (indentation_width % 2);
    for (unsigned i = 1; i < level / 2; ++i)
      stream << ' ';
}

void writer::node(string const &s, bool quote, bool block) {
  if (p->last_block)
    --p->level;

  quote |= s.find_first_of(" ,", 0) != string::npos;
  
  if (block || s.find_first_of(L'\n', 0) != string::npos)
    return p->print_as_block(s);

  if (p->newline) p->stream << L'\n';
  p->indent();
  if (quote) p->stream << '"';
  p->stream << s;
  if (quote) p->stream << '"';

  if (p->last_block) {
    ++p->level;
    p->last_block = false;
  }

  p->newline = true;
  p->level_change = false;
}

static string::const_iterator pos_to_it(string const &s, 
                                         string::size_type p) {
  if (p == string::npos)
    return s.end();
  return s.begin() + p;
}

void writer::impl::print_as_block(string const &s) {
  if (newline) stream << L'\n';

  indent();
  stream << '`' << L'\n';

  level += 2;
  
  string::size_type incr = s.find_first_not_of(' ', 0);
  string::size_type pos = 0;
  while (pos_to_it(s, pos) != s.end()) {  
    string::size_type start = pos;
    pos = s.find_first_of('\n', pos);
    if (pos != string::npos)
      ++pos;
      
    indent();
    stream << string(pos_to_it(s, start), pos_to_it(s, pos));

    if (s.find_first_not_of(' ', pos) < incr)
      throw non_conformant_block();
  };

  level -= 2;

  last_block = true;
  level_change = false;
  newline = true;
}

void writer::down() {
  p->level += 2;
  p->level_change = true;
}

void writer::up() {
  p->level -= 2;
  p->level_change = true;
}

void writer::comment(string const &s, bool l) {
  if (p->level_change || l) { 
    p->stream << '\n';
    p->indent();
  } else
    p->stream << ' ';
  p->stream << '#' << s;
  p->newline = true;
}

void writer::end_document() {
  if (p->newline)
    p->stream << '\n';
}

}}}
