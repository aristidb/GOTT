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

#include "repatcher_by_name.hpp"
#include "repatch.hpp"
#include <gott/range_algo.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

using gott::tdl::structure::repatcher_getter;
using gott::tdl::structure::repatcher_by_name_t;

repatcher_getter::repatcher_getter() {}
repatcher_getter::~repatcher_getter() {}

repatcher_by_name_t::repatcher_by_name_t() {}
repatcher_by_name_t::~repatcher_by_name_t() {}

repatcher_by_name_t &gott::tdl::structure::repatcher_by_name() {
  static repatcher_by_name_t instance;
  return instance;
}

void repatcher_by_name_t::add(
    string const &name, 
    boost::function<repatcher_getter *()> const &func) {
  repo[name] = func;
}

repatcher_getter *repatcher_by_name_t::get_alloc(string const &name) const {
  std::map<string, boost::function<repatcher_getter *()> >::const_iterator it =
    repo.find(name);
  if (it == repo.end())
    throw 0;
  return it->second();
}

repatcher_getter *repatcher_by_name_t::chain_alloc() const {
  struct getter : repatcher_getter {
    getter(repatcher_by_name_t const &r) 
    : ref(r), pos(outer), result(new repatcher_chain) {}
    repatcher_by_name_t const &ref;
    string what;
    boost::scoped_ptr<repatcher_getter> where;
    enum { outer, inner1, inner2 } pos;
    repatcher_chain *result;
    unsigned inner2_level;
    void begin() {
      switch (pos) {
      case outer:
        pos = inner1; break;
      case inner1:
        where.reset(ref.get_alloc(what));
        inner2_level = 0;
        pos = inner2; break;
      case inner2:
        ++inner2_level;
        where->begin();
      }
    }
    void end() {
      switch (pos) {
      case inner2:
        if (inner2_level) {
          where->end();
          --inner2_level;
        } else {
          result->push_back_alloc(where->result_alloc());
          where.reset();
          pos = inner1;
        }
        break;
      case inner1:
        pos = outer; break;
      case outer:
        fail();
      }
    }
    void data(xany::Xany const &x) {
      switch (pos) {
      case outer:
        fail();
      case inner1:
        what = xany::Xany_cast<string>(x); break;
      case inner2:
        where->data(x); break;
      }
    }
    void add_tag(string const &s) {
      if (pos == inner2)
        where->add_tag(s);
    }
    void fail() {
      throw std::invalid_argument("chain_alloc: invalid input");
    }
    repatcher *result_alloc() const {
      return result;
    }
  };
  return new getter(*this);
}
