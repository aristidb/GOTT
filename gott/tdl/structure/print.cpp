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
#include <gott/string/string.hpp>

using std::basic_ostream;
using gott::xany::Xany;
using gott::tdl::structure::direct_print;

template<class C>
class direct_print<C>::IMPL {
public:
  IMPL(basic_ostream<C> &o, unsigned s)
  : out(o), level(-s), step(s), line_ended(true), tag_printed(false) {}
  
  basic_ostream<C> &out;
  unsigned level;
  unsigned const step;
  bool line_ended, tag_printed;    
};

template<class C>
direct_print<C>::direct_print(basic_ostream<C> &o, unsigned s) 
: p(new IMPL(o, s)) {}

template<class C> direct_print<C>::~direct_print() {}

template<class C> void direct_print<C>::begin() {
  p->level += p->step;
  if (!p->line_ended)
    p->out << '\n';
  p->line_ended = true;
}

template<class C> void direct_print<C>::end() {
  p->level -= p->step;
}

template<class C> void direct_print<C>::data(Xany const &x) {
  for (unsigned i = 0; i < p->level; ++i)
    p->out << ' ';
  if (x.empty())
    p->out << '-';
  else
    p->out << x;
  p->tag_printed = false;
  p->line_ended = false;
}

template<class C> void direct_print<C>::add_tag(string const &s) {
  if (p->tag_printed)
    p->out << ", ";
  else
    p->out << " : ";
  p->out << s;
  p->tag_printed = true;
}

std::ostream &gott::tdl::structure::operator<<(std::ostream &o, 
                                               copyable_structure const &s) {
  direct_print<char> p(o);
  s.copy_to(p);
  return o;
}

#ifdef HAVE_WIDE_STDLIB
std::wostream &gott::tdl::structure::operator<<(std::wostream &o, 
                                                copyable_structure const &s) {
  direct_print<wchar_t> p(o);
  s.copy_to(p);
  return o;
}
#endif
