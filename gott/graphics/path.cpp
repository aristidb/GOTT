// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Non-immediate graphics library
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

#include "path.hpp"
#include "font.hpp"
#include "transformations.hpp"
#include <gott/string/string.hpp>
#include <gott/range_algo.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>

using gott::string;
namespace graphics = gott::graphics;
using graphics::path;
using graphics::point;

struct path::node {
  virtual ~node() {}
  virtual node *append(node *other);
  virtual node *clone() const = 0;

  friend inline path::node *new_clone(path::node const &n) {
    return n.clone();
  }
};

typedef boost::scoped_ptr<path::node> node_p;

path::~path() { delete head; }
path::path(path const &o) : head(o.head->clone()) {}

namespace { 

template<class T>
struct concrete_node : path::node {
  path::node *clone() const { return new T(static_cast<T const &>(*this)); }
};

struct appended : concrete_node<appended> {
  typedef boost::ptr_vector<path::node> children_list;

  appended(std::auto_ptr<boost::ptr_vector<path::node> > c) : children(c) {}

  appended(appended const &o) 
  : concrete_node<appended>(), children(o.children.clone()) {}

  path::node *append(path::node *other) {
    children.push_back(other);
    return this;
  }

  children_list children;
};

}

path::node *path::node::append(node *other) {
  ::appended::children_list c;
  c.push_back(this);
  c.push_back(other);
  return new ::appended(c.release());
}

path &path::operator+=(path const &o) {
  head = head->append(o.head->clone());
  return *this;
}

namespace {
struct arc : concrete_node<arc> {
  arc(point const &c, double r, double a1, double a2, bool n = false)
  : center(c), radius(r), angle1(a1), angle2(a2), negative(n) {}
  point center;
  double radius;
  double angle1, angle2;
  bool negative;
};
}

path &path::arc_p(point const &center, double radius, 
    double angle1, double angle2) {
  head = head->append(new arc(center, radius, angle1, angle2));
  return *this;
}

path &path::arc_negative_p(point const &center, double radius,
    double angle1, double angle2) {
  head = head->append(new arc(center, radius, angle1, angle2, true));
  return *this;
}

namespace {
struct curve_to : concrete_node<curve_to> {
  curve_to(point const &c1, point const &c2, point const &e)
  : control1(c1), control2(c2), end(e) {}
  point control1, control2;
  point end;
};
}

path &path::curve_to_p(point const &c1, point const &c2, point const &e) {
  head = head->append(new curve_to(c1, c2, e));
  return *this;
}

namespace {
struct line_to : concrete_node<line_to> {
  line_to(point const &c) : control(c) {}
  point control;
};
}

path &path::line_to(point const &p) {
  head = head->append(new ::line_to(p));
  return *this;
}

namespace {
struct move_to : concrete_node<move_to> {
  move_to(point const &c) : control(c) {}
  point control;
};
}

path &path::move_to(point const &p) {
  head = head->append(new ::move_to(p));
  return *this;
}

namespace {
struct text : concrete_node<text> {
  text(string const &d, graphics::font const &s) : data(d), style(s) {}
  string data;
  graphics::font style;
};
}

path &path::text(string const &data, font const &style) {
  head = head->append(new ::text(data, style));
  return *this;
}

namespace {
  using graphics::transformations;
struct transform : concrete_node<transform> {
  transform(transformations const &t, path::node *c) : transf(t), child(c) {}
  transform(transform const &o)
  : concrete_node<transform>(), transf(o.transf), child(o.child->clone()) {}
  transformations transf;
  node_p child;
};
}

path &path::transform(transformations const &transf) {
  head = new ::transform(transf, head);
  return *this;
}
