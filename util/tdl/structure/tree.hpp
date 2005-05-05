// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

#ifndef GOTT_UTIL_TDL_STRUCTURE_TREE_HPP
#define GOTT_UTIL_TDL_STRUCTURE_TREE_HPP

#include <gott/util/tdl/structure/structure.hpp>

namespace gott {
namespace util {
namespace tdl {
namespace structure {

// Class tree
// The default implementation of the tdl data structure
class tree : public revocable_structure, public copyable_structure {
  struct node;

public: // Iterators
  class tagged_iterator;

  // Class iterator
  // The default iterator
  // Corresponds to a node in the tree
  class iterator {
  public:
    typedef size_t size_type;
    
    EXPORT size_type size() const;
      // the number of children
    
    EXPORT iterator operator[](size_type n) const;
      // the n-th child

    EXPORT tagged_iterator with_tag(std::wstring const &) const;
      // all children with the given tag
    
    EXPORT iterator first_child() const;
      // the first child of the node

    EXPORT iterator next() const;
      // the right sibling of the node

    EXPORT iterator up() const;
      // the parental node
    
    EXPORT xany::Xany const &get_data() const;
      // reads the node's data

    EXPORT std::list<std::wstring> const &get_tags() const;
      // reads all the node's tags

    operator bool() { return n; }
      // is the iterator valid?

    typedef tagged_iterator tagged;

    EXPORT bool contents_equal(iterator const &);

  private:  
    iterator();
    iterator(boost::intrusive_ptr<node> const &);
    friend class tree;
    friend class tagged_iterator;

    boost::intrusive_ptr<node> n;
  };

  // Class tagged_iterator
  // Corresponds to a node in a set of nodes with a common tag
  EXPORT class tagged_iterator {
  public:
    EXPORT tagged_iterator(tagged_iterator const &);
    EXPORT ~tagged_iterator();
    
    EXPORT void operator++();
      // go to the next node

    EXPORT operator bool() const;
      // is the iterator valid?

    EXPORT iterator get() const;
      // get a normal iterator
  
  private:
    class IMPL;
    boost::scoped_ptr<IMPL> const pIMPL;

    tagged_iterator(IMPL *);
    friend class iterator;
  };

  EXPORT iterator get_root() const;
    // get an iterator for the root node

private: // Inherited interface
  // Don't call these directly (exported symbols for no use is nothing I like)
  void add_tag(std::wstring const &);
  void set_tags(std::list<std::wstring> const &);
  void data(xany::Xany const &);
  void begin();
  void end();
  
  pth point();
  void revert(pth const &);
  void get_rid_of(pth const &);

public:
  EXPORT void copy_to(writable_structure &) const;
  EXPORT bool operator==(tree const &o) const;

public:
  EXPORT tree();
  EXPORT ~tree();

#ifdef DEBUG
  EXPORT void dump(std::wostream &);
#endif

private:
  typedef unsigned long tag;
  
  static boost::intrusive_ptr<node> erase(boost::intrusive_ptr<node>);
  boost::intrusive_ptr<node> root, pos;
  tag current_tag;
  hashd::hash_map<tag, boost::intrusive_ptr<node> > tagpos;

  EXPORT friend void intrusive_ptr_add_ref(node *);
  EXPORT friend void intrusive_ptr_release(node *);

  bool del_since(boost::intrusive_ptr<node> &, tag);
  void foo();
};

inline bool operator!=(tree const &lhs, tree const &rhs) {
  return !(lhs == rhs);
}

}}}}

#endif
