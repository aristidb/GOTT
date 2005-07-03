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

#ifndef GOTT_BASE_PLUGIN_QID_HPP
#define GOTT_BASE_PLUGIN_QID_HPP

#include <gott/util/misc/commonheaders.hpp>

namespace gott {
namespace base {
namespace plugin {

class QID {
public:
  QID(char const *);
  QID(wchar_t const *);
  QID(std::string const &);
  QID(std::wstring const &);

  QID parent() const;

  QID child(char const *) const;
  QID child(wchar_t const *) const;
  QID child(std::string const &) const;
  QID child(std::wstring const &) const;
};

bool operator==(QID const &, QID const &);
bool operator!=(QID const &, QID const &);

}}}

#endif
