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
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#ifndef GOTT_TDL_STRUCTURE_REPATCH_HPP
#define GOTT_TDL_STRUCTURE_REPATCH_HPP

#include "structure.hpp"
#include "../resource.hpp"
#include <vector>
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/optional/optional_fwd.hpp>
#include <boost/function.hpp>

namespace tdl {

namespace schema { class rule_t; }

namespace structure {

/**
 * This class represents a structure patcher.
 */
class GOTT_EXPORT repatcher {
public:
  repatcher();
  virtual ~repatcher() = 0;
  virtual repatcher *clone() const = 0;
  virtual writable_structure *deferred_write(writable_structure &) const = 0;
  virtual writable_structure *inverse(writable_structure &) const;
};

inline repatcher *new_clone(repatcher const &r) {
  return r.clone();
}

template<class T> class concrete_repatcher : public repatcher {
  repatcher *clone() const { return new T(*static_cast<T const *>(this)); }
};

class GOTT_EXPORT simple_repatcher_context : public writable_structure {
protected:
  writable_structure &target;

public:
  simple_repatcher_context(writable_structure &x) : target(x) {}
  ~simple_repatcher_context();
  void begin(source_position const &);
  void end();
  void data(gott::xany::Xany const &x);
  void add_tag(gott::string const &t);
};

class GOTT_EXPORT repatch_nothing : public concrete_repatcher<repatch_nothing> {
public:
  repatch_nothing();
  ~repatch_nothing();
  writable_structure *deferred_write(writable_structure &) const;
};

class GOTT_EXPORT repatcher_chain : public concrete_repatcher<repatcher_chain> {
public:
  repatcher_chain();
  ~repatcher_chain();
  repatcher_chain(repatcher_chain const &);
  void push_back(repatcher const &r);
  void push_back_alloc(repatcher *);
  writable_structure *deferred_write(writable_structure &) const;
private:
  boost::ptr_vector<repatcher> el;
};

class GOTT_EXPORT repatcher_getter : public writable_structure {
public:
  repatcher_getter();
  ~repatcher_getter();
  /**
   * Allocate a repatcher of the promised type with the given arguments.
   * Only call this once!
   */
  virtual repatcher *result_alloc() const = 0;
};

class GOTT_EXPORT repatcher_getter_factory : public resource {
public:
  repatcher_getter_factory();
  ~repatcher_getter_factory();

  static gott::atom const kind;
  virtual repatcher_getter *alloc() const = 0;
  virtual schema::rule_t parameter_schema() const = 0;
private:
  gott::atom get_kind() const { return kind; }
};

GOTT_EXPORT repatcher_getter *repatcher_by_name();

}}

#endif
