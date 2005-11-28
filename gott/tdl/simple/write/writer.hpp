// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Simple TDL writer
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

#ifndef GOTT_UTIL_TDL_SIMPLE_WRITER_H
#define GOTT_UTIL_TDL_SIMPLE_WRITER_H

#include <gott/visibility.hpp>
#include <iosfwd>
#include <boost/scoped_ptr.hpp>

namespace gott {
namespace tdl {
namespace simple {

class writer {
public:
  GOTT_EXPORT writer(std::ostream &, unsigned = 4);
  GOTT_EXPORT void down();
  GOTT_EXPORT void up();
  GOTT_EXPORT void node(std::string const &content, 
                   bool quote = false, 
                   bool block = false);
  GOTT_EXPORT void comment(std::string const &content, bool on_new_line);
  GOTT_EXPORT void end_document();

  GOTT_EXPORT class non_conformant_block {};

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

}}}

#endif
