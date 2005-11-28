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

#ifndef GOTT_UTIL_TDL_STRUCTURE_PRINT_HPP
#define GOTT_UTIL_TDL_STRUCTURE_PRINT_HPP

#include "structure.hpp"
#include <boost/scoped_ptr.hpp>

namespace gott {
namespace tdl {
namespace structure {

/**
 * A structure writer callback directly writing to a stream. Does not support
 * backtracking, of course.
 */
template<class Ch>
class direct_print : public writable_structure {
public:
  /**
   * Constructor.
   * \param out The stream to write to.
   * \param step The indentation width.
   */
  GOTT_EXPORT direct_print(std::basic_ostream<Ch> &out, unsigned step = 4);
  GOTT_EXPORT ~direct_print();

private:
  void begin();
  void end();
  void data(xany::Xany const &);
  void add_tag(string const &);

  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

#ifdef HAVE_WIDE_STDLIB
/**
 * Print a (copyable) structure object to a (wide) stream.
 * \param o The stream to write to.
 * \param s The structure to print.
 */
GOTT_EXPORT
std::wostream &operator<<(std::wostream &o, copyable_structure const &s);
#endif

/**
 * Print a (copyable) structure object to a (normal) stream.
 * @copydoc operator<<(std::wostream &, copyable_structure const &)
 */
GOTT_EXPORT
std::ostream &operator<<(std::ostream &o, copyable_structure const &s);

}}}

#endif
