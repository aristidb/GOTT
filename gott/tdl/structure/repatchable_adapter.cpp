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

#include "repatchable_adapter.hpp"
#include <gott/tdl/structure/repatch.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>
#include <cassert>

#define VERBOSE

#ifdef VERBOSE
#include <iostream>
#endif

using namespace tdl::structure;

template<class T>
class tdl::structure::repatchable_adapter_impl {
public:
  T &out;
  boost::ptr_vector<writable_structure> indirections;
  std::vector<boost::shared_ptr<repatcher const> > tracker;

  writable_structure &current() {
    if (indirections.empty())
      return out;
    else
      return indirections.back();
  }

  void add(boost::shared_ptr<repatcher const> const &x) {
    tracker.push_back(x);
    indirections.push_back(x->deferred_write(current()));
  }

  void remove(boost::shared_ptr<repatcher const> const &x) {
    #ifndef NDEBUG
    assert(!tracker.empty());
    assert(tracker.back() == x);
    #else
    (void)x;
    #endif
    tracker.pop_back();
    assert(!indirections.empty());
    indirections.pop_back();
  }

  repatchable_adapter_impl(T &out) : out(out) {}
};

repatchable_adapter::repatchable_adapter(revocable_structure &out)
  : p(new impl(out)) {}
repatchable_adapter::~repatchable_adapter() {}

void repatchable_adapter::begin(tdl::source_position const &w) {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter:" << this << ">:begin" << std::endl;
#endif
  p->current().begin(w);
}

void repatchable_adapter::end() {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter:" << this << ">:end" << std::endl;
#endif
  p->current().end();
}

void repatchable_adapter::add_tag(gott::string const &s) {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter:" << this << ">:add_tag " << s
    << std::endl;
#endif
  p->current().add_tag(s);
}

void repatchable_adapter::data(gott::Xany const &x) {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter:" << this << ">:data ";
  try {
    std::cout << x;
  } catch (...) {}
  std::cout << std::endl;
#endif
  p->current().data(x);
}

void repatchable_adapter::add_repatcher(
    boost::shared_ptr<repatcher const> const &x) {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter:" << this << ">:add_repatcher " <<
    x.get() << " : _Z" << typeid(*x.get()).name() << std::endl;
#endif
  p->add(x);
}

void repatchable_adapter::remove_repatcher(
    boost::shared_ptr<repatcher const> const &x) {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter:" << this << ">:remove_repatcher " <<
    x.get() << std::endl;
#endif
  p->remove(x);
}

void repatchable_adapter::add_repatcher2(
    boost::shared_ptr<repatcher const> const &x) {
  p->out.add_repatcher2(x);
}

void repatchable_adapter::remove_repatcher2(
    boost::shared_ptr<repatcher const> const &x) {
  p->out.remove_repatcher2(x);
}

revocable_structure::pth repatchable_adapter::point() {
  return p->out.point();
}

void repatchable_adapter::revert(pth x) {
  p->out.revert(x);
}

void repatchable_adapter::get_rid_of(pth x) {
  p->out.get_rid_of(x);
}

repatchable_adapter2::repatchable_adapter2(writable_structure &out)
  : p(new impl(out)) {}
repatchable_adapter2::~repatchable_adapter2() {}

void repatchable_adapter2::begin(tdl::source_position const &w) {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter2:" << this << ">:begin" << " <> " << &p->current() << std::endl;
#endif
  p->current().begin(w);
}

void repatchable_adapter2::end() {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter2:" << this << ">:end" << " <> " << &p->current() << std::endl;
#endif
  p->current().end();
}

void repatchable_adapter2::add_tag(gott::string const &s) {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter2:" << this << ">:add_tag " << s << " <> " << &p->current() << std::endl;
#endif
  p->current().add_tag(s);
}

void repatchable_adapter2::data(gott::Xany const &x) {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter2:" << this << ">:data ";
  try {
    std::cout << x;
  } catch (...) {}
  std::cout << " <> " << &p->current() << std::endl;
#endif
  p->current().data(x);
}

void repatchable_adapter2::add_repatcher2(
    boost::shared_ptr<repatcher const> const &x) {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter2:" << this << ">:add_repatcher2 " <<
    x.get() << " : _Z" << typeid(*x.get()).name() << std::endl;
#endif
  p->add(x);
}

void repatchable_adapter2::remove_repatcher2(
    boost::shared_ptr<repatcher const> const &x) {
#ifdef VERBOSE
  std::cout << "<repatchable_adapter2:" << this << ">:remove_repatcher2 " <<
    x.get() << std::endl;
#endif
  p->remove(x);
}
