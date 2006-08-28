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

#include "repatch.hpp"
#include "../exceptions.hpp"
#include "../source_position.hpp"
#include <gott/range_algo.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

using namespace tdl::structure;
using gott::string;
using gott::Xany;

repatcher::repatcher() {}
repatcher::~repatcher() {}

simple_repatcher_context::~simple_repatcher_context() {}

void simple_repatcher_context::begin(source_position const &w) {
  target.begin(w);
}

void simple_repatcher_context::end() {
  target.end();
}

void simple_repatcher_context::data(Xany const &x) {
  target.data(x);
}

void simple_repatcher_context::add_tag(string const &s) {
  target.add_tag(s);
}

repatch_nothing::repatch_nothing() {}
repatch_nothing::~repatch_nothing() {}

writable_structure *
repatch_nothing::deferred_write(writable_structure &s) const {
  return new simple_repatcher_context(s);
}

repatcher_chain::repatcher_chain() {}
repatcher_chain::~repatcher_chain() {}

repatcher_chain::repatcher_chain(repatcher_chain const &o) 
: concrete_repatcher<repatcher_chain>(), el(o.el.clone()) {}

void repatcher_chain::push_back(repatcher const &r) {
  el.push_back(r.clone());
}

void repatcher_chain::push_back_alloc(repatcher *r) {
  el.push_back(r);
}

writable_structure *
repatcher_chain::deferred_write(writable_structure &s) const {
  struct context : public writable_structure {
    boost::ptr_vector<writable_structure> out;
    context(boost::ptr_vector<repatcher> const &el, writable_structure &target){
      out.reserve(el.size());
      int i = el.size() - 1;
      out.push_back(el[i].deferred_write(target));
      while (--i >= 0)
        out.push_back(el[i].deferred_write(out.back()));
    }
    void begin(source_position const &w) { out.back().begin(w); }
    void end() { out.back().end(); }
    void data(gott::xany::Xany const &x) { out.back().data(x); }
    void add_tag(gott::string const &s) { out.back().add_tag(s); }
  };
  if (el.empty())
    return repatch_nothing().deferred_write(s);
  else
    return new context(el, s);
}

repatcher_getter::repatcher_getter() {}
repatcher_getter::~repatcher_getter() {}

repatcher_getter *tdl::structure::repatcher_by_name() {
  struct getter : repatcher_getter {
    getter() 
    : pos(outer), result(new repatcher_chain) {}

    string what;
    boost::scoped_ptr<repatcher_getter> where;
    enum { outer, inner1, inner2 } pos;
    repatcher_chain *result;
    unsigned inner2_level;

    repatcher_getter *one_alloc(string const &) const {
      throw 0;
    }

    void begin(source_position const &w) {
      switch (pos) {
      case outer:
        pos = inner1; break;
      case inner1:
        where.reset(one_alloc(what));
        what = string();
        inner2_level = 0;
        pos = inner2; break;
      case inner2:
        ++inner2_level;
        where->begin(w);
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
        if (what != string()) {
          begin(source_position());
          end();
        }
        pos = outer; break;
      case outer:
        fail();
      }
    }
    void data(gott::xany::Xany const &x) {
      switch (pos) {
      case outer:
        fail();
      case inner1:
        what = gott::xany::Xany_cast<string>(x); break;
      case inner2:
        where->data(x); break;
      }
    }
    void add_tag(string const &s) {
      if (pos == inner2)
        where->add_tag(s);
    }
    void fail() {
      throw tdl_error("TDL Structure repatcher loader",
          "non-sensible arguments");
    }
    repatcher *result_alloc() const {
      return result;
    }
  };
  return new getter;
}

