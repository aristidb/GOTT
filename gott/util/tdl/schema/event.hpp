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

#ifndef GOTT_TDL_SCHEMA_EVENT_HPP
#define GOTT_TDL_SCHEMA_EVENT_HPP

#include <iosfwd>
#include <gott/util/string/string.hpp>
#include <gott/util/visibility.hpp>
#include <boost/variant.hpp>
#include <ntl.h>

namespace gott {
namespace tdl {
namespace schema {

class item;

/**
 * Schema events. Interesting for rule_t-writers.
 */
namespace ev {

/**
 * Interface event for TDL tokens and schema notifications.
 */
class event {
public:
  /**
   * Notify a item of this event.
   * Calls item::play with a reference to *this.
   * \param r The item to notify.
   */
  virtual bool play(item &r) const = 0;

  /**
   * @internal
   * Print the event to a stream.
   * Meant for debugging purposes.
   * \param s The stream to write to.
   */
  virtual void print(std::ostream &s) const = 0;

  virtual event *clone() const = 0;

  virtual ~event() = 0;
};

/**
 * Print an event to a stream.
 */
inline std::ostream &operator<<(std::ostream &s, event const &e) {
  e.print(s);
  return s;
}

/// The base class for token events.
class token : public event {
};

/// The event class for the simple::parser::begin_parse token.
class begin_parse : public token {
public:
  bool play(item &r) const;
  void print(std::ostream &s) const;
  event *clone() const { return new begin_parse; }
};

/// The event class for the simple::parser::down token.
class down : public token {
public:
  bool play(item &r) const;
  void print(std::ostream &s) const;
  event *clone() const { return new down; }
};

/// The event class for the simple::parser::node token.
class node : public token {
  string data;
public:
  node(string const &);
    // construct a node with the given data

  bool play(item &r) const;
  void print(std::ostream &s) const;
  event *clone() const { return new node(*this); }

  string const &get_data() const GOTT_EXPORT;
    // get the data of this node-event
};

/// The event class for the simple::parser::up token.
class up : public token {
public:
  bool play(item &r) const;
  void print(std::ostream &s) const;
  event *clone() const { return new up; }
};

/// The event class for the simple::parser::end_parse token.
class end_parse : public token {
public:
  bool play(item &r) const;
  void print(std::ostream &s) const;
  event *clone() const { return new end_parse; }
};

typedef boost::variant<begin_parse, down, node, up, end_parse> token_t;

NTL_MOVEABLE(token_t);

token const &get(token_t const &);

/// The base class for notification events.
class notification : public event {
};

/// The event class for a succeeded child.
class child_succeed : public notification {
public:
  bool play(item &r) const;
  void print(std::ostream &s) const;
  event *clone() const { return new child_succeed; }
};

/// The event class for a failed child.
class child_fail : public notification {
public:
  bool play(item &r) const;
  void print(std::ostream &s) const;
  event *clone() const { return new child_fail; }
};

}}}}

#endif
