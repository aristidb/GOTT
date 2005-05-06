// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

#include "print.hpp"

using std::wostream;
using std::wstring;
using std::list;
using gott::util::xany::Xany;
using gott::util::tdl::structure::direct_print;

direct_print::direct_print(wostream &o, unsigned s) 
: out(o), level(-s), step(s), line_ended(true), tag_printed(false) {}

direct_print::~direct_print() {}

void direct_print::begin() {
  level += step;
  if (!line_ended)
    out << L'\n';
  line_ended = true;
}

void direct_print::end() {
  level -= step;
}

void direct_print::data(Xany const &x) {
  for (unsigned i = 0; i < level; ++i)
    out << L' ';
  if (x.empty())
    out << L'-';
  else
    out << x;
  tag_printed = false;
  line_ended = false;
}

void direct_print::add_tag(wstring const &s) {
  if (tag_printed)
    out << L", ";
  else
    out << L" : ";
  out << s;
  tag_printed = true;
}

void direct_print::set_tags(list<wstring> const &l) {
  for (list<wstring>::const_iterator it = l.begin(); it != l.end(); ++it)
    add_tag(*it);
}
