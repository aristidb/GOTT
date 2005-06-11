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

/**
 * Interface event for TDL tokens and schema notifications.
 */
// A "storable" method invocation.
class event {
public:
  /**
   * Notify a rule of this event.
   * Calls rule::play with a reference to *this.
   * \param r The rule to notify.
   */
  virtual bool play(rule &r) const = 0;

  /**
   * @internal
   * Print the event to a stream.
   * Meant for debugging purposes.
   * \param s The stream to write to.
   */
  virtual void print(std::wostream &s) const = 0;

  virtual ~event() = 0;
};

/**
 * Print an event to a stream.
 */
inline std::wostream &operator<<(std::wostream &s, event const &e) {
  e.print(s);
  return s;
}

/// The base class for token events.
class token : public event {
};

/// The event class for the simple::parser::begin_parse token.
class begin_parse : public token {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

/// The event class for the simple::parser::down token.
class down : public token {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

/// The event class for the simple::parser::node token.
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

/// The event class for the simple::parser::up token.
class up : public token {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

/// The event class for the simple::parser::end_parse token.
class end_parse : public token {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

typedef boost::variant<begin_parse, down, node, up, end_parse> token_t;

token const &get(token_t const &);

/// The base class for notification events.
class notification : public event {
};

/// The event class for a succeeded child.
class child_succeed : public notification {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

/// The event class for a failed child.
class child_fail : public notification {
public:
  bool play(rule &r) const;
  void print(std::wostream &s) const;
};

}}}}}

#endif
