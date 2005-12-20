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

#ifndef GOTT_UTIL_TDL_STRUCTURE_REPATCH_HPP
#define GOTT_UTIL_TDL_STRUCTURE_REPATCH_HPP

#include "structure.hpp"

namespace gott {
namespace tdl {
namespace structure {

/**
 * This class represents a structure patcher.
 */
class GOTT_EXPORT repatcher {
public:
  repatcher();
  virtual ~repatcher() = 0;
  virtual repatcher *clone() const = 0;
  virtual writable_structure *deferred_write(writable_structure &) const = 0;
};

template<class T> class concrete_repatcher : public repatcher {
  repatcher *clone() const { return new T(*static_cast<T const *>(this)); }
};

class GOTT_EXPORT simple_repatcher_context : public writable_structure {
protected:
  writable_structure &target;

public:
  simple_repatcher_context(writable_structure &x) : target(x) {}
  ~simple_repatcher_context();
  void begin();
  void end();
  void data(xany::Xany const &x);
  void add_tag(string const &t);
};

class GOTT_EXPORT repatch_nothing : public concrete_repatcher<repatch_nothing> {
public:
  repatch_nothing();
  ~repatch_nothing();
  writable_structure *deferred_write(writable_structure &) const;
};

class GOTT_EXPORT repatcher_chain : public concrete_repatcher<repatcher_chain> {
public:
  repatcher_chain();
  ~repatcher_chain();
  repatcher_chain(repatcher_chain const &);
  void push_back(repatcher const &r);
  writable_structure *deferred_write(writable_structure &) const;
private:
  Vector<repatcher *> el;
};

}}}

#endif
