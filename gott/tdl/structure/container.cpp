// Copyright (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
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

#include "container.hpp"
#include <vector>
#include <stack>
#include <boost/bind.hpp>
#include <gott/range_algo.hpp>

using gott::xany::Xany;
using gott::tdl::structure::container;

class container::impl {
public:
  impl() : current(&root) {
    parents.push(0);
  }
  class node {
  public:
    Xany data;
    std::vector<string> tags;
    std::vector<node> children;

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

void container::begin() {
  p->parents.push(p->current);
  p->current->children.push_back(impl::node());
  p->current = &p->current->children.back();
}

void container::end() {
  p->current = p->parents.top();
  p->parents.pop();
}

void container::add_tag(string const &s) {
  p->current->tags.push_back(s);
}

void container::data(xany::Xany const &data) {
  p->current->data = data;
}

void container::copy_to(writable_structure &w) const {
  for_each(range(p->root.children), boost::bind(&impl::node::write, _1, &w));
}

bool container::operator==(container const &x) const {
  if (this == &x)
    return true;
  return p->root == x.p->root;
}


void container::impl::node::write(writable_structure *w) const {
  w->begin();
  w->data(data);
  for_each(range(tags), boost::bind(&writable_structure::add_tag, w, _1));
  for_each(range(children), boost::bind(&node::write, _1, w));
  w->end();
}

bool container::impl::node::operator==(node const &o) const {
  if (data != o.data)
    return false;
  if (range(tags) != range(o.tags))
    return false;
  return range(children) == range(o.children);
}
