// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
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
#include <ostream>
#include <list>
#include <sstream>
#include <gott/util/nstring/nstring.hpp>
#include <gott/util/autoconv.hpp>

using std::wostream;
using std::wstring;
using std::list;
using gott::xany::Xany;
using gott::tdl::structure::direct_print;

class direct_print::IMPL {
public:
  IMPL(wostream &o, unsigned s)
  : out(o), level(-s), step(s), line_ended(true), tag_printed(false) {}
  
  wostream &out;
  unsigned level;
  unsigned const step;
  bool line_ended, tag_printed;    
};

direct_print::direct_print(wostream &o, unsigned s) 
: p(new IMPL(o, s)) {}

direct_print::~direct_print() {}

void direct_print::begin() {
  p->level += p->step;
  if (!p->line_ended)
    p->out << L'\n';
  p->line_ended = true;
}

void direct_print::end() {
  p->level -= p->step;
}

void direct_print::data(Xany const &x) {
  for (unsigned i = 0; i < p->level; ++i)
    p->out << L' ';
  if (x.empty())
    p->out << L'-';
  else
    p->out << x;
  p->tag_printed = false;
  p->line_ended = false;
}

void direct_print::add_tag(nstring const &s) {
  if (p->tag_printed)
    p->out << L", ";
  else
    p->out << L" : ";
  p->out << s;
  p->tag_printed = true;
}

void direct_print::set_tags(list<nstring> const &l) {
  for (list<nstring>::const_iterator it = l.begin(); it != l.end(); ++it)
    add_tag(*it);
}

std::wostream &gott::tdl::structure::operator<<(std::wostream &o, 
                                                copyable_structure const &s) {
  direct_print p(o);
  s.copy_to(p);
  return o;
}

std::ostream &gott::tdl::structure::operator<<(std::ostream &o, 
                                               copyable_structure const &s) {
  std::wostringstream wo;
  wo << s;
  return o << wo.str();
}
