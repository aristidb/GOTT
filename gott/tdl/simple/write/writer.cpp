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

using std::ostream;
using std::string;

namespace gott {
namespace tdl {
namespace simple {
  
class writer::IMPL {
public:
  IMPL(ostream &os, unsigned i)
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

writer::writer(ostream &os, unsigned i) : p(new IMPL(os, i)) {}

void writer::IMPL::indent() {
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

void writer::IMPL::print_as_block(string const &s) {
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
