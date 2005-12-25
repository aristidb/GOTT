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

#ifndef GOTT_TDL_STRUCTURE_REPATCHER_BY_NAME_HPP
#define GOTT_TDL_STRUCTURE_REPATCHER_BY_NAME_HPP

#include "structure.hpp"
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace gott {
namespace tdl {
namespace structure {

class repatcher;

class GOTT_EXPORT repatcher_getter : public writable_structure {
public:
  repatcher_getter();
  ~repatcher_getter();
  /**
   * Allocate a repatcher of the promised type with the given arguments.
   * Only call this once!
   */
  virtual repatcher *result_alloc() const = 0;
};

class repatcher_by_name_t : boost::noncopyable {
public:
  repatcher_by_name_t();
  ~repatcher_by_name_t();

  GOTT_EXPORT
  void add(string const &, boost::function<repatcher_getter *()> const &);
  GOTT_EXPORT
  repatcher_getter *get_alloc(string const &) const;
  GOTT_EXPORT
  repatcher_getter *chain_alloc() const;

private:
  VectorMap<string, boost::function<repatcher_getter *()> > repo;
};

GOTT_EXPORT repatcher_by_name_t &repatcher_by_name();

}}}

#endif
