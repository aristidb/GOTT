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

#ifndef GOTT_UTIL_TDL_STRUC_STRUC_H
#define GOTT_UTIL_TDL_STRUC_STRUC_H

#include <gott/visibility.hpp>
#include <gott/xany/xany.hpp>

namespace gott {
class string;
}

namespace tdl {
class source_position;

namespace structure {

/**
 * The interface for writable structures. Just includes regular flow without
 * backtracking.
 */
class writable_structure {
public:
  /**
   * Set the data of the current node.
   * \param x The value to write.
   */
  virtual void data(gott::xany::Xany const &x) = 0;

  /**
   * Start a child of the current node.
   */
  virtual void begin(source_position const &) = 0;

  /**
   * End a node.
   */
  virtual void end() = 0;

  /**
   * Add a tag to the node.
   * \param t The tag to add.
   */
  virtual void add_tag(gott::string const &t) = 0;

  GOTT_EXPORT virtual ~writable_structure() = 0;
};

/**
 * The interface for revocable (backtracking) structures.
 */
class revocable_structure : public writable_structure {
public:
  /// The time-point as used by a backtracking-aware structure writer.
  typedef std::ptrdiff_t pth;

  /**
   * Acquire a time-point. Don't forget to call get_rid_of.
   * \return The fresh time-point.
   */
  virtual pth point() = 0;
  
  /**
   * Revert to an acquired time-point.
   * \param pt The time-point to revert to.
   */
  virtual void revert(pth pt) = 0;

  /**
   * "Destroy" a time-point.
   * \param pt The time point to get rid of.
   */
  virtual void get_rid_of(pth pt) = 0;

  GOTT_EXPORT virtual ~revocable_structure() = 0;
};

/**
 * The interface for copyable structures. Just includes the copying portion.
 */
class copyable_structure {
public:
  /**
   * Copy the structure to another structure object.
   * \param ws The structure object to copy to.
   */
  virtual void copy_to(writable_structure &ws) const = 0;

  GOTT_EXPORT virtual ~copyable_structure() = 0;
};

/**
 * Directly convert a TDL document into a structure.
 * \param stream The stream to read from.
 * \param structure The structure to fill.
 */
GOTT_EXPORT 
void direct_struc(std::istream &stream, writable_structure &structure);

}}

#endif
