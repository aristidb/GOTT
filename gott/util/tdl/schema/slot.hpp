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

#ifndef GOTT_UTIL_TDL_SCHEMA_SLOT_HPP
#define GOTT_UTIL_TDL_SCHEMA_SLOT_HPP

#include "item.hpp"
#include <boost/function.hpp>

namespace gott {
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
    exact = 64
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
    multiple = list | some | minimum | maximum | exact | range | function,
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
  bool prefix_optional() const;
  /// A variant of the object that does not accept 0 children.
  slotcfg no_optional() const;
  /// The mode the object is in.
  mode get_mode() const GOTT_EXPORT;

  /// Add an element.
  void add() GOTT_EXPORT;
  /// Cancel a sequence.
  void cancel() GOTT_EXPORT;
  /// Accept or not.
  item::expect expectation() const GOTT_EXPORT;

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

}}}


#endif
