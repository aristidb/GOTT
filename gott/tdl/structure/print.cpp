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

#include "print.hpp"
#include <ostream>
#include <gott/string/string.hpp>

using std::basic_ostream;
using gott::xany::Xany;
using tdl::structure::direct_print;

template<class C>
class direct_print<C>::impl {
public:
  impl(basic_ostream<C> &o, unsigned s)
  : out(o), level(-s), step(s), line_ended(true), tag_printed(false) {}
  
  basic_ostream<C> &out;
  unsigned level;
  unsigned const step;
  bool line_ended, tag_printed;    
};

template<class C>
direct_print<C>::direct_print(basic_ostream<C> &o, unsigned s) 
: p(new impl(o, s)) {}

template<class C> direct_print<C>::~direct_print() {}

template<class C> void direct_print<C>::begin(source_position const &) {
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

template<class C> void direct_print<C>::add_tag(gott::string const &s) {
  if (p->tag_printed)
    p->out << ", ";
  else
    p->out << " : ";
  p->out << s;
  p->tag_printed = true;
}

std::ostream &tdl::structure::operator<<(std::ostream &o,
    copyable_structure const &s) {
  direct_print<char> p(o);
  s.copy_to(p);
  return o;
}

#ifdef HAVE_WIDE_STDLIB
std::wostream &tdl::structure::operator<<(std::wostream &o,
    copyable_structure const &s) {
  direct_print<wchar_t> p(o);
  s.copy_to(p);
  return o;
}
#endif
