// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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

#ifndef GOTT_UTIL_TDL_SCHEMA_ITEM_HPP
#define GOTT_UTIL_TDL_SCHEMA_ITEM_HPP

#include "ev_fwd.hpp"
#include <boost/scoped_ptr.hpp>
#include <gott/visibility.hpp>

namespace gott {
class string;
}

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
   * A nice name for the item.
   */
  virtual gott::string name() const = 0;

  /**
   * A longer name for the item with some extra information.
   */
  virtual gott::string long_name() const;

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
  GOTT_LOCAL void finish();

  virtual ~item() = 0;

  // Properties
  /**
   * Get the attributes associated with this rule_t.
   */
  GOTT_EXPORT rule_attr_t const &attributes() const;

protected:
  match &matcher();
    // return a reference to the match-object that embedded us

  item(rule_attr_t const &a, match &m);
    // an implementation must supply attributes

private:
  class impl;
  boost::scoped_ptr<impl> pimpl;

  GOTT_LOCAL item();
};

}}

#endif
