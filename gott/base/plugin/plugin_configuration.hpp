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

#ifndef GOTT_BASE_PLUGIN_PLUGIN_CONFIGURATION_HPP
#define GOTT_BASE_PLUGIN_PLUGIN_CONFIGURATION_HPP

#include <gott/util/visibility.hpp>
#include <boost/scoped_ptr.hpp>

namespace gott {
namespace xany { class Xany; }

namespace plugin {

class hook;
class QID;

class plugin_configuration {
public:
  plugin_configuration() GOTT_EXPORT;
  ~plugin_configuration() GOTT_EXPORT;

  void add_param(QID const &, xany::Xany const &) GOTT_EXPORT;
  xany::Xany const &find_param(QID const &) const GOTT_EXPORT;

  void add_hook(QID const &, hook const &) GOTT_EXPORT;
  hook const &find_hook(QID const &) const GOTT_EXPORT;

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

}}

#endif
