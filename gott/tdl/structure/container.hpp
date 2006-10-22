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
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#ifndef GOTT_TDL_STRUCTURE_CONTAINER_HPP
#define GOTT_TDL_STRUCTURE_CONTAINER_HPP

#include "structure.hpp"
#include <boost/scoped_ptr.hpp>

namespace tdl {
namespace structure {

/**
 * Generic container.
 */
class container : public writable_structure, public copyable_structure {
public:
  /// Constructor.
  GOTT_EXPORT container();

  /// Copy-Constructor.
  GOTT_EXPORT container(container const &);

  /// Destructor.
  GOTT_EXPORT ~container();

  /// Assignment-Operator.
  GOTT_EXPORT void operator=(container const &);

  /// Clear the container.
  GOTT_EXPORT void clear();

  /// Compare to another container.
  GOTT_EXPORT bool operator==(container const &) const;

  /// Compare to another container.
  inline bool operator!=(container const &o) const {
    return !operator==(o);
  }

  /// Copy contents to another structure.
  GOTT_EXPORT void copy_to(writable_structure &w) const;

  /// Swap contents with another container.
  GOTT_EXPORT void swap(container &x);

private:
  void begin(source_position const &);
  void end();
  void add_tag(gott::string const &);
  void data(gott::xany::Xany const &);

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}

#endif
