// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
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

#ifndef GOTT_UTIL_TDL_STRUCTURE_TYPES_ENUMERATION_HPP
#define GOTT_UTIL_TDL_STRUCTURE_TYPES_ENUMERATION_HPP

#include "../repatch.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace structure {

class GOTT_EXPORT repatch_enumeration : public repatcher {
public:
  repatch_enumeration(std::vector<std::wstring> const &);
  ~repatch_enumeration();
  writable_structure *deferred_write(writable_structure &) const;

private:
  std::vector<std::wstring> alternatives;
};

}}}}

#endif
