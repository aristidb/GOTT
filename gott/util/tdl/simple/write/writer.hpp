// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Simple TDL writer
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

#ifndef GOTT_UTIL_TDL_SIMPLE_WRITER_H
#define GOTT_UTIL_TDL_SIMPLE_WRITER_H

#include <gott/util/misc/commonheaders.hpp>

namespace gott {
namespace util {
namespace tdl {
namespace simple {

class writer {
public:
  EXPORT writer(std::wostream &, unsigned = 4);
  EXPORT void down();
  EXPORT void up();
  EXPORT void node(std::wstring const &content, 
                   bool quote = false, 
                   bool block = false);
  EXPORT void comment(std::wstring const &content, bool on_new_line);
  EXPORT void end_document();

  EXPORT class non_conformant_block {};

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

}}}}
#endif
