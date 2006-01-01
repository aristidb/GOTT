// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#ifndef GOTT_UTIL_TDL_SCHEMA_PARSE_POSITION_HPP
#define GOTT_UTIL_TDL_SCHEMA_PARSE_POSITION_HPP

#include "ev_fwd.hpp"
#include <gott/tdl/structure/structure.hpp>
#include <utility>
#include <boost/scoped_ptr.hpp>

namespace gott {
namespace tdl {

namespace schema {

/**
 * Used for backtracking.
 */
class positioning {
public:
  positioning(structure::revocable_structure &);
  ~positioning();

  typedef std::pair<long, structure::revocable_structure::pth> id;

  id current() GOTT_EXPORT;
  id current_readonly() GOTT_EXPORT;
  void seek(id const &) GOTT_EXPORT;
  void seek_and_forget(id const &) GOTT_EXPORT;
  void forget(id const &) GOTT_EXPORT;

  struct GOTT_EXPORT acceptor {
    virtual void operator()(ev::token const &) = 0;
    virtual ~acceptor() = 0;
  };

  void replay(acceptor &);
  bool want_replay() const;

  template<class T>
  void add(T const &); // add a token but don't consume it yet

  void consume();
  void pass(); 

  bool proceeded(id const &) const GOTT_EXPORT; // from the unconsumed token

#ifdef DEBUG
  void debug_dump() const;
#endif

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

}}}

NTL_MOVEABLE(gott::tdl::schema::positioning::id);

#endif
