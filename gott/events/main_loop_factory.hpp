// Copyright (C) 2005-2006 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT main loop
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

#ifndef GOTT_BASE_EVENTS_MAIN_LOOP_FACTORY
#define GOTT_BASE_EVENTS_MAIN_LOOP_FACTORY

#include <gott/visibility.hpp>
#include <boost/noncopyable.hpp>

namespace gott { class QID;
namespace events {
class loop_requirement;
class main_loop;

class main_loop_factory : boost::noncopyable {
public:
  GOTT_EXPORT main_loop_factory();
  GOTT_EXPORT ~main_loop_factory();
  GOTT_EXPORT bool try_add(loop_requirement const &);
  GOTT_EXPORT main_loop *get_alloc() const;

public: // internal
  bool try_add_feature(QID const &);
};

}}

#endif
