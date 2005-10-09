// Simple writer for tdl data files
//
// Copyright (C) 2004 Aristid Breitkreuz
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "writer.hpp"
#include <ostream>

using std::wostream;
using std::wstring;

namespace gott {
namespace tdl {
namespace simple {
  
class writer::IMPL {
public:
  IMPL(wostream &os, unsigned i)
  : stream(os), level(0), indentation_width(i), newline(false), 
    last_block(false) {}

  wostream &stream;
  unsigned level;
  unsigned indentation_width;
  bool newline;
  bool last_block;
  bool level_change;

  void indent();
  void print_as_block(wstring const &s);
};

writer::writer(wostream &os, unsigned i) : p(new IMPL(os, i)) {}

void writer::IMPL::indent() {
  for (unsigned i = 2; i < level; ++i)
    for (unsigned j = 0; j < indentation_width / 2; ++j)
      stream << L' ';

  if (indentation_width % 2);
    for (unsigned i = 1; i < level / 2; ++i)
      stream << L' ';
}

void writer::node(wstring const &s, bool quote, bool block) {
  if (p->last_block)
    --p->level;

  quote |= s.find_first_of(L" ,", 0) != wstring::npos;
  
  if (block || s.find_first_of(L'\n', 0) != wstring::npos)
    return p->print_as_block(s);

  if (p->newline) p->stream << L'\n';
  p->indent();
  if (quote) p->stream << L'"';
  p->stream << s;
  if (quote) p->stream << L'"';

  if (p->last_block) {
    ++p->level;
    p->last_block = false;
  }

  p->newline = true;
  p->level_change = false;
}

static wstring::const_iterator pos_to_it(wstring const &s, 
                                         wstring::size_type p) {
  if (p == wstring::npos)
    return s.end();
  return s.begin() + p;
}

void writer::IMPL::print_as_block(wstring const &s) {
  if (newline) stream << L'\n';

  indent();
  stream << L'`' << L'\n';

  level += 2;
  
  wstring::size_type incr = s.find_first_not_of(L' ', 0);
  wstring::size_type pos = 0;
  while (pos_to_it(s, pos) != s.end()) {  
    wstring::size_type start = pos;
    pos = s.find_first_of(L'\n', pos);
    if (pos != wstring::npos)
      ++pos;
      
    indent();
    stream << wstring(pos_to_it(s, start), pos_to_it(s, pos));

    if (s.find_first_not_of(L' ', pos) < incr)
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

void writer::comment(wstring const &s, bool l) {
  if (p->level_change || l) { 
    p->stream << L'\n';
    p->indent();
  } else
    p->stream << L' ';
  p->stream << L'#' << s;
  p->newline = true;
}

void writer::end_document() {
  if (p->newline)
    p->stream << L'\n';
}

}}}
