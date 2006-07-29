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

#ifndef GOTT_UTIL_TDL_SCHEMA_SLOT_HPP
#define GOTT_UTIL_TDL_SCHEMA_SLOT_HPP

#include "item.hpp"
#include <boost/function.hpp>

namespace tdl {
namespace schema {

/**
 * "Repetition definition".
 * Specifies the number of children of a type a container may have.
 */
class slotcfg {
public:
  /// Simple modes (no parameter).
  enum simple_mode {
    /// Accept a single child
    one = 1,
    /// Optionally accept a single child
    optional = 2, 
    /// Accept at least one child
    some = 4,
    /// Accept any number of children
    list = 8
  };

  /// Sized modes (one \c std::size_t parameter).
  enum sized_mode {
    /// Accept minimally \arg x children
    minimum = 16,
    /// Accept maximally \arg x children
    maximum = 32,
    /// Accept exactly \arg x children
    exactly = 64
  };

  /// Ranged modes (two \c std::size_t parameters).
  enum range_mode {
    /// Accept between \arg x and \arg y children. Borders inclusive.
    range = 128
  };

  /// Function modes (one \c callback parameter).
  enum function_mode {
    /// Accept as the specified by the function.
    function = 256
  };

  /** 
   * Classes of modes. Because modes are powers of two, they can be combined
   * into bitsets.
   */
  enum mode_classes {
    /// No possible mode.
    none = 0,
    /// All modes that accept maximally one child.
    single = one | optional, 
    /// All modes that accept more than one child.
    multiple = list | some | minimum | maximum | exactly | range | function,
    /// All modes.
    all = single | multiple
  };

  /**
   * The combined mode type.
   * @see simple_mode
   * @see sized_mode
   * @see range_mode
   * @see function_mode
   */
  typedef unsigned mode;

  /**
   * The callback specifying whether to accept or not.
   * \param items The number of items.
   * \return See item::expect.
   */
  typedef boost::function<item::expect (std::size_t items)> callback;

  /**
   * Initialize a slotcfg object accepting one child.
   */
  slotcfg() GOTT_EXPORT;

  /// Constructor.
  slotcfg(simple_mode) GOTT_EXPORT;

  /// Constructor.
  slotcfg(sized_mode, std::size_t) GOTT_EXPORT;

  /// Constructor.
  slotcfg(range_mode, std::size_t, std::size_t) GOTT_EXPORT;

  /// Constructor.
  slotcfg(function_mode, callback const &) GOTT_EXPORT;

  /// Copy-Constructor.
  slotcfg(slotcfg const &) GOTT_EXPORT;

  /// Destructor.
  ~slotcfg() GOTT_EXPORT;

  void operator=(slotcfg const &o) GOTT_EXPORT;

  /// Whether the object accepts 0 children.
  bool prefix_optional() const GOTT_EXPORT;
  /// A variant of the object that does not accept 0 children.
  slotcfg no_optional() const GOTT_EXPORT;
  /// The mode the object is in.
  mode get_mode() const GOTT_EXPORT;

  bool operator==(slotcfg const &o) const GOTT_EXPORT;

  /// Add an element.
  void add() GOTT_EXPORT;
  /// Cancel a sequence.
  void cancel() GOTT_EXPORT;
  /// Accept or not.
  item::expect expectation() const GOTT_EXPORT;

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

namespace {
inline slotcfg one() { return slotcfg(); }
inline slotcfg optional() { return slotcfg(slotcfg::optional); }
inline slotcfg some() { return slotcfg(slotcfg::some); }
inline slotcfg list() { return slotcfg(slotcfg::list); }

inline slotcfg at_least(std::size_t n) { return slotcfg(slotcfg::minimum, n); }
inline slotcfg no_more_than(std::size_t n){return slotcfg(slotcfg::maximum, n);}
inline slotcfg exactly(std::size_t n) { return slotcfg(slotcfg::exactly, n); }

inline slotcfg between(std::size_t min, std::size_t max) {
  return slotcfg(slotcfg::range, min, max);
}
}

}}


#endif
