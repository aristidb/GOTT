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

#ifndef GOTT_UTIL_TDL_STRUCTURE_PRINT_HPP
#define GOTT_UTIL_TDL_STRUCTURE_PRINT_HPP

#include "structure.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace structure {

class direct_print : public writable_structure {
public:
  EXPORT direct_print(std::wostream &out, unsigned step = 4);
  EXPORT ~direct_print();

private:
  void begin();
  void end();
  void data(xany::Xany const &);
  void add_tag(std::wstring const &);
  void set_tags(std::list<std::wstring> const &);

  std::wostream &out;
  unsigned level;
  unsigned const step;
  bool line_ended, tag_printed;
};

inline 
std::wostream &operator<<(std::wostream &o, copyable_structure const &s) {
  direct_print p(o);
  s.copy_to(p);
  return o;
}

inline 
std::ostream &operator<<(std::ostream &o, copyable_structure const &s) {
  std::wostringstream wo;
  wo << s;
  return o << wo.str();
}

template<class Ch>
std::basic_string<Ch> to_string(copyable_structure const &s) {
  std::basic_ostringstream<Ch> o;
  o << s;
  return o.str();
}

}}}}

#endif
