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

#include "container.hpp"
#include <gott/tdl/source_position.hpp>
#include <vector>
#include <stack>
#include <boost/bind.hpp>
#include <gott/range_algo.hpp>

using gott::string;
using gott::xany::Xany;
using tdl::structure::container;

class container::impl {
public:
  impl() : root(source_position()), current(&root) {
    parents.push(0);
  }
  class node {
  public:
    node(source_position const &w) : where(w) {}

    Xany data;
    std::vector<string> tags;
    std::vector<node> children;
    source_position where;

    void write(writable_structure *w) const;
    bool operator==(node const &o) const;
    bool operator!=(node const &o) const {
      return !operator==(o);
    }
  };
  node root;
  node *current;
  
  std::stack<node *> parents;
};

container::container() : p(new impl) {}

container::container(container const &x)
: writable_structure(), copyable_structure(), p(new impl) {
  x.copy_to(*this);
}

container::~container() {}

void container::operator=(container const &x) {
  if (this == &x)
    return;
  clear();
  x.copy_to(*this);
}

void container::clear() {
  p.reset(new impl);
}

void container::begin(source_position const &w) {
  p->parents.push(p->current);
  p->current->children.push_back(impl::node(w));
  p->current = &p->current->children.back();
}

void container::end() {
  p->current = p->parents.top();
  p->parents.pop();
}

void container::add_tag(string const &s) {
  p->current->tags.push_back(s);
}

void container::data(Xany const &data) {
  p->current->data = data;
}

void container::copy_to(writable_structure &w) const {
  using namespace gott;
  for_each(range(p->root.children), boost::bind(&impl::node::write, _1, &w));
}

bool container::operator==(container const &x) const {
  if (this == &x)
    return true;
  return p->root == x.p->root;
}


void container::impl::node::write(writable_structure *w) const {
  using namespace gott;
  w->begin(where);
  w->data(data);
  for_each(range(tags), boost::bind(&writable_structure::add_tag, w, _1));
  for_each(range(children), boost::bind(&node::write, _1, w));
  w->end();
}

bool container::impl::node::operator==(node const &o) const {
  if (data != o.data)
    return false;
  if (gott::range(tags) != gott::range(o.tags))
    return false;
  return gott::range(children) == gott::range(o.children);
}
