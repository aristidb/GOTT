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

#ifndef GOTT_UTIL_TDL_SCHEMA_PARSE_POSITION_HPP
#define GOTT_UTIL_TDL_SCHEMA_PARSE_POSITION_HPP

#include "ev_fwd.hpp"
#include <gott/tdl/structure/structure.hpp>
#include <utility>
#include <boost/scoped_ptr.hpp>

namespace gott {
namespace tdl {

namespace schema {

/**
 * Used for backtracking.
 */
class positioning {
public:
  positioning(structure::revocable_structure &);
  ~positioning();

  typedef std::pair<long, structure::revocable_structure::pth> id;

  id current() GOTT_EXPORT;
  id current_readonly() GOTT_EXPORT;
  void seek(id const &) GOTT_EXPORT;
  void seek_and_forget(id const &) GOTT_EXPORT;
  void forget(id const &) GOTT_EXPORT;

  struct GOTT_EXPORT acceptor {
    virtual void operator()(ev::token const &) = 0;
    virtual ~acceptor() = 0;
  };

  void replay(acceptor &);
  bool want_replay() const;

  template<class T>
  void add(T const &); // add a token but don't consume it yet

  void consume();
  void pass(); 

  bool proceeded(id const &) const GOTT_EXPORT; // from the unconsumed token

#ifdef DEBUG
  void debug_dump() const;
#endif

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}}

#endif
