// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
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

#include "structure.hpp"
#include "tree.hpp"
#include "print.hpp"
#include <gott/range_algo.hpp>
#include <boost/bind.hpp>
#include <sstream>
#include <gott/autoconv.hpp>
#include <gott/string/string.hpp>

using std::pair;
using boost::intrusive_ptr;
using boost::scoped_ptr;
using gott::xany::Xany;
using gott::string;

namespace stru = gott::tdl::structure;
using stru::writable_structure;
using stru::revocable_structure;
using stru::copyable_structure;
using stru::tree;
using stru::direct_print;

writable_structure::~writable_structure() {}
revocable_structure::~revocable_structure() {}
copyable_structure::~copyable_structure() {}

class tree::IMPL {
public:
  IMPL() : current_tag(0) {}

  typedef unsigned long tag;
  
  static boost::intrusive_ptr<node> erase(boost::intrusive_ptr<node>);
  boost::intrusive_ptr<node> root, pos;
  tag current_tag;
  VectorMap<tag, boost::intrusive_ptr<node> > tagpos;

  bool del_since(boost::intrusive_ptr<node> &, tag);
};

NTL_MOVEABLE(intrusive_ptr<tree::node>);

struct tree::node {
  size_t use_count;
  Xany data;
  Vector<string> tags;
  intrusive_ptr<node> child0, lastchild, dad, sibling, last;
  size_t size;
  IMPL::tag ttag;
  
  class index;
  scoped_ptr<index> indx;

  class index {
    typedef Vector<intrusive_ptr<node> > v_pn;
    typedef VectorMap<string, intrusive_ptr<node> > hmm_spn;
    
    v_pn by_no;
    hmm_spn by_name;
    intrusive_ptr<node> parent;
    bool built;

  public:
    typedef hmm_spn const *tag_container_ptr;
    typedef int tag_position;
    typedef pair<tag_container_ptr, tag_position> tag_range_type;
    
    index(intrusive_ptr<node> const &n) : parent(n), built(false) {}

    void build_all() {
      if (!built) {
        by_no.Reserve(parent->size);
        for (intrusive_ptr<node> i = parent->child0; i; i = i->sibling) {
          by_no.Add(i);
          add_entry(i);
        }
        built = true;
      }
    }

    void add_entry(intrusive_ptr<node> const &x) {
      for (Vector<string>::iterator i= x->tags.begin(); i != x->tags.end();++i)
        by_name.Add(*i, x);
    }

    intrusive_ptr<node> get_at(size_t i) {
      return by_no[i];
    }

    tag_range_type with_name(string const &n) {
      return tag_range_type(&by_name, by_name.Find(n));
    }
  };
  
  node(IMPL::tag t) : use_count(0), size(0), ttag(t) {}

  bool equals(node const &) const;
};

void stru::intrusive_ptr_add_ref(tree::node *p) {
  ++p->use_count;
}

void stru::intrusive_ptr_release(tree::node *p) {
  if (!--p->use_count)
    delete p;
}

intrusive_ptr<tree::node> tree::IMPL::erase(intrusive_ptr<node> x) {
  --x->dad->size;

  if (x->sibling) 
    x->sibling->last = x->last;

  if (x->last) 
    x->last->sibling = x->sibling;
  
  if (x->dad->child0 == x) 
    x->dad->child0 = x->sibling;
    
  if (x->dad->lastchild == x) 
    x->dad->lastchild = x->last;
  
  return x->sibling;
}

tree::tree() : p(new IMPL) {}
tree::~tree() {}

void tree::begin() {
  if (!p->pos) {
    p->root = intrusive_ptr<node>(new node(p->current_tag));
    p->pos = p->root;
    return;
  }
  
  intrusive_ptr<node> nn(new node(p->current_tag));
  nn->dad = p->pos;

  if (!p->pos->child0) {
    p->pos->child0 = nn;
    p->pos->lastchild = nn;
  } else {
    p->pos->lastchild->sibling = nn;
    nn->last = p->pos->lastchild;
    p->pos->lastchild = nn;
  }
  
  ++p->pos->size;
  
  p->pos = nn;
}

void tree::end() {
  if (p->pos != p->root)
    p->pos = p->pos->dad;
}

void tree::data(Xany const &x) {
  p->pos->data = x;
}

void tree::add_tag(string const &s) {
  p->pos->tags.push_back(s);
}

revocable_structure::pth tree::point() {
  p->tagpos.Add(++p->current_tag, p->pos);
  return p->current_tag;
}

void tree::revert(revocable_structure::pth const &mp) {
  p->del_since(p->root, mp);
  p->pos = p->tagpos.Get(mp);
}

void tree::get_rid_of(revocable_structure::pth const &x) {
  p->tagpos.RemoveKey(x);
}

bool tree::IMPL::del_since(intrusive_ptr<node> &n, tag t) {
  for (intrusive_ptr<node> i = n->child0; i;)
    if (del_since(i, t)) 
      i = erase(i);
    else
      i = i->sibling;

  return !n->child0 && n->ttag >= t;
}

tree::iterator::iterator() {}
tree::iterator::iterator(boost::intrusive_ptr<node> const &nn) : n(nn) {}

tree::iterator::size_type tree::iterator::size() const {
  return n->size;
}

tree::iterator tree::iterator::operator[](size_type x) const { 
  if (!n->indx)
    n->indx.reset(new tree::node::index(n));
  n->indx->build_all();
  return n->indx->get_at(x);
}

tree::iterator tree::iterator::first_child() const { return n->child0; }

tree::iterator tree::iterator::up() const { return n->dad; }
tree::iterator tree::iterator::next() const { return n->sibling; }

Xany const &tree::iterator::get_data() const { return n->data; }
Vector<string> const &tree::iterator::get_tags() const { return n->tags; }

tree::iterator tree::get_root() const { return p->root; }

bool tree::iterator::contents_equal(iterator const &o) const {
  return n->equals(*o.n);
}

class tree::tagged_iterator::IMPL {
public:
  tree::node::index::tag_range_type range;

  IMPL(tree::node::index::tag_range_type const &r) 
    : range(r) {}
};

tree::tagged_iterator tree::iterator::with_tag(string const &s) const {
  if (!n->indx)
    n->indx.reset(new tree::node::index(n));
  n->indx->build_all();
  return new tagged_iterator::IMPL(n->indx->with_name(s));
}

tree::tagged_iterator::tagged_iterator(IMPL *p) : pIMPL(p) {}
tree::tagged_iterator::tagged_iterator(tagged_iterator const &o) 
: pIMPL(new IMPL(*o.pIMPL)) {}

tree::tagged_iterator::~tagged_iterator() {}

void tree::tagged_iterator::operator++() {
  pIMPL->range.second = pIMPL->range.first->FindNext(pIMPL->range.second);
}

tree::tagged_iterator::operator bool() const {
  return pIMPL->range.second > 0;
}

tree::iterator tree::tagged_iterator::get() const {
  return (*pIMPL->range.first)[pIMPL->range.second];
}

void tree::copy_to(writable_structure &target) const {
  struct {
    writable_structure &target;
    
    void operator()(intrusive_ptr<node> p) {
      target.begin();
      target.data(p->data);
      for_each(range(p->tags), bind(&writable_structure::add_tag, &target,_1));

      for (p = p->child0; p; p = p->sibling)
        operator()(p);

      target.end();
    }
  } visitor = {target};
  visitor(p->root);
}

bool tree::operator==(tree const &o) const {
  return p->root->equals(*o.p->root);
}

bool tree::node::equals(node const &o) const {
  if (data != o.data) 
    return false;
  if (tags.GetCount() != o.tags.GetCount())
    return false;
  if (!equal(range(tags), range(o.tags)))
    return false;

  intrusive_ptr<node> p = child0, q = o.child0;
  while (p && q) {
    if (!p->equals(*q))
      return false;
    p = p->sibling;
    q = q->sibling;
  }
  return !(p || q);
}

template<class Ch>
std::basic_ostream<Ch> &
stru::operator<<(std::basic_ostream<Ch> &s, tree::iterator const &i) {
  direct_print<Ch> p(s);
  struct printer {
    writable_structure &p;
    void operator() (tree::iterator const &x) {
      p.begin();
        p.data(x.get_data());
        for (Vector<string>::const_iterator it = x.get_tags().begin();
             it != x.get_tags().end(); ++it)
          p.add_tag(*it);
        for (tree::iterator i = x.first_child(); i; i = i.next())
          operator()(i);
      p.end();
    }
  } x = {p};
  x(i);
  return s;
}

template std::ostream &
  stru::operator<< <char>(std::ostream &, tree::iterator const &);
template std::wostream &
  stru::operator<< <wchar_t>(std::wostream&,tree::iterator const &);

#ifdef DEBUG
#include <iostream>

using std::wostream;

void tree::dump(wostream &stream) {
  struct visitor {
    wostream &out;
    unsigned level;
    intrusive_ptr<node> pp;
    void ind() {
      for (unsigned x = 0; x < level; ++x)
        out << L' ';
    }
    void operator()(intrusive_ptr<node> p) {
      ind();
      if (p == pp)
        out << L'*';
      if (p->data.empty())
        out << L'-';
      else
        out << p->data;

      out << L'(' << p->ttag << L')';
      out << L" : ";
      for (Vector<string>::iterator it = p->tags.begin(); 
           it != p->tags.end(); ++it)
        out << *it << L' ';
      out << L"{\n";
      level += 2;
      for (p = p->child0; p; p = p->sibling)
        operator()(p);
      level -= 2;
      ind();
      out << L"}\n";
    }
    visitor(wostream &oo, intrusive_ptr<node> p) : out(oo), level(0), pp(p) {}
  };
  visitor v(stream, p->pos);
  v(p->root);
}
#endif
