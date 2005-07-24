// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
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

#ifndef GOTT_UTIL_TDL_STRUCTURE_REPATCH_HPP
#define GOTT_UTIL_TDL_STRUCTURE_REPATCH_HPP

#include "structure.hpp"

namespace gott {
namespace util {
namespace tdl {
namespace structure {

class repatch_context;

/**
 * This class represents a structure patcher.
 */
class EXPORT repatcher {
public:
  repatcher();
  virtual ~repatcher() = 0;
  virtual repatch_context *deferred_write(writable_structure &) = 0;
};

class EXPORT repatch_context : public writable_structure {
public:
  virtual ~repatch_context() = 0;
};

}}}}

#endif
