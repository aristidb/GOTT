/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef GOTT_TDL_SCHEMA_EVENT_HPP
#define GOTT_TDL_SCHEMA_EVENT_HPP

#include <iosfwd>
#include <gott/string/string.hpp>
#include <gott/visibility.hpp>
#include <boost/variant.hpp>

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
