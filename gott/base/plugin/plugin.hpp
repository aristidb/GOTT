// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT Plugin/Loadable Module Engine
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

#ifndef GOTT_BASE_PLUGIN_PLUGIN_HPP
#define GOTT_BASE_PLUGIN_PLUGIN_HPP

#include <gott/visibility.hpp>

namespace gott {
namespace plugin {

class system_configuration;
class plugin_configuration;
class hook;
class QID;

class GOTT_EXPORT plugin_base {
public:
  virtual ~plugin_base() = 0;
  virtual void add(QID const &point, hook const &extension) = 0;
};

typedef plugin_base *(*plugin_builder)(system_configuration &, 
                                       plugin_configuration const &);

}}

#endif
