// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#ifndef GOTT_TDL_SCHEMA_EVENT_HPP
#define GOTT_TDL_SCHEMA_EVENT_HPP

#include <gott/util/misc/commonheaders.hpp>

namespace gott {
namespace util {
namespace tdl {
namespace schema {

class rule;

namespace ev {

// Interface event
// A "storable" method invocation.
class event {
public:
  virtual bool play(rule &r) const = 0;
    // play this event on the given rule via r.play(*this)

  virtual void print(std::wostream &s) const = 0;
    // print to stream
  
  virtual ~event() = 0;
};

inline std::wostream &operator<<(std::wostream &s, event const &e) {
  e.print(s);
  return s;
}

class token : public event {
};

class begin_parse : public token {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

class down : public token {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

class node : public token {
  std::wstring data;
public:
  node(std::wstring const &);
    // construct a node with the given data
  
  bool play(rule &r) const;
  void print(std::wostream &s) const;

  std::wstring const &get_data() const EXPORT;
    // get the data of this node-event
};

class up : public token {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

class end_parse : public token {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

typedef boost::variant<begin_parse, down, node, up, end_parse> token_t;

token const &get(token_t const &);

class notification : public event {
};

class child_succeed : public notification {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

class child_fail : public notification {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

}}}}}

#endif
