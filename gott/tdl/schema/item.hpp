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

#ifndef GOTT_UTIL_TDL_SCHEMA_ITEM_HPP
#define GOTT_UTIL_TDL_SCHEMA_ITEM_HPP

#include "ev_fwd.hpp"
#include <boost/scoped_ptr.hpp>
#include <gott/visibility.hpp>

namespace gott {
class string;

namespace tdl {
namespace structure { class repatcher; }

namespace schema {

class match;
class rule_attr_t;

/**
 * The base class of the implementation of a rule_t-matcher object.
 * Used via match::add or via match's constructor. Attention: Those methods
 * require a rule_t-factory, which in turn will create a item object.
 */
class GOTT_EXPORT item {
public:

  /**
   * The possible expectations of a living item object.
   */
  enum expect {
    /// Expect nothing (when finished or empty).
    nothing = 1,
    /// Maybe accept something (when optionally accepting more).
    maybe = 2,
    /// Need input.
    need = -3,
    /// Never occurs (I hope). 
    over_filled = -4
  };

  static bool accept_more(expect e) {
    return e != nothing;
  }

  /**
   * Returns the current expectation.
   */
  virtual expect expectation() const = 0;

  /**
   * A nice name for the rule_t.
   */
  virtual string name() const = 0;

  // Event handlers
  // Tokens
  /**
   * Tries to accept an ev:begin_parse token. Adjusts expectation.
   * Default implementation: Do nothing.
   * \return
   *   - \c true on success
   *   - \c false on failure
   */
  virtual bool play(ev::begin_parse const &);

  /**
   * Tries to accept an ev::down token.
   * @copydoc play(ev::begin_parse const &)
   */
  virtual bool play(ev::down const &);

  /**
   * Tries to accept an ev::node token.
   * @copydoc play(ev::begin_parse const &)
   */
  virtual bool play(ev::node const &);

  /**
   * Tries to accept an ev::up token.
   * @copydoc play(ev::begin_parse const &)
   */
  virtual bool play(ev::up const &);

  /**
   * Tries to accept an ev::end_parse token.
   * @copydoc play(ev::begin_parse const &)
   */
  virtual bool play(ev::end_parse const &);

  // Notifications
  /**
   * Tries to accept an ev::child_succeed event.
   * @copydoc play(ev::begin_parse const &)
   */
  virtual bool play(ev::child_succeed const &);
  
  /**
   * Tries to accept an ev::child_fail event.
   * @copydoc play(ev::begin_parse const &)
   */
  virtual bool play(ev::child_fail const &);

  /**
   * Called for undifferentiated events.
   */
  virtual bool play_other();

  /**
   * Called if some event(s) is/are required by a item but not immediately 
   * found.
   */
  virtual bool miss_events(ev::event const &, unsigned);

  /// @internal
  void finish() GOTT_LOCAL;

  virtual ~item() = 0;

  // Properties
  /**
   * Get the attributes associated with this rule_t.
   */
  rule_attr_t const &attributes() const GOTT_EXPORT;

protected:
  match &matcher();
    // return a reference to the match-object that embedded us

  item(rule_attr_t const &a, match &m);
    // an implementation must supply attributes

private:
  class impl;
  boost::scoped_ptr<impl> pimpl;

  item() GOTT_LOCAL;
};

}}}

#endif
