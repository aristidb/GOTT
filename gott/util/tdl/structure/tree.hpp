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

#ifndef GOTT_UTIL_TDL_STRUCTURE_TREE_HPP
#define GOTT_UTIL_TDL_STRUCTURE_TREE_HPP

#include "structure.hpp"
#include <boost/intrusive_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <ntl.h>

namespace gott {
namespace tdl {
namespace structure {

/**
 * The default implementation of the (whole) structure concept.
 */
class tree : public revocable_structure, public copyable_structure {
public: // unfortunately
  struct node;

public: // Iterators
  class tagged_iterator;

  /**
   * The default iterator.
   * Corresponds to a node in the tree.
   */
  class iterator : Moveable<iterator> {
  public:
    typedef size_t size_type;
    
    /**
     * The number of children.
     */
    GOTT_EXPORT size_type size() const;
    
    /**
     * Gets a child by index. Creates the index table as necessary.
     * \param n The index to get an element at.
     */
    GOTT_EXPORT iterator operator[](size_type n) const;

    /**
     * Gets children by tag.
     * \param tag The tag to search for.
     */
    GOTT_EXPORT tagged_iterator with_tag(nstring const &tag) const;
    
    /**
     * Gets the first child.
     */
    GOTT_EXPORT iterator first_child() const;

    /**
     * Gets the iterator with the next-higher position. This is: the right 
     * sibling.
     */
    GOTT_EXPORT iterator next() const;

    /**
     * Gets the parental node.
     */
    GOTT_EXPORT iterator up() const;
    
    /**
     * Gets the data.
     */
    GOTT_EXPORT xany::Xany const &get_data() const;

    /**
     * Gets all tags.
     */
    GOTT_EXPORT Vector<nstring> const &get_tags() const;

    /**
     * Checks whether the iterator is valid.
     */
    operator bool() const { return n; }

    /**
     * Checks whether two iterators are equal by structure and content.
     * \param other The structure to compare with.
     */
    GOTT_EXPORT bool contents_equal(iterator const &other) const;

    /**
     * @see query::selection
     */
    typedef tagged_iterator tagged;

  private:  
    iterator();
    iterator(boost::intrusive_ptr<node> const &);
    friend class tree;
    friend class tagged_iterator;

    boost::intrusive_ptr<node> n;
  };

  /**
   * Iterates through a set of nodes with a common tag.
   */
  GOTT_EXPORT class tagged_iterator {
  public:
    GOTT_EXPORT tagged_iterator(tagged_iterator const &);
    GOTT_EXPORT ~tagged_iterator();
    
    /**
     * Go to the next node with the same tag.
     */
    GOTT_EXPORT void operator++();

    /**
     * Checks whether the iterator is still valid.
     */
    GOTT_EXPORT operator bool() const;

    /**
     * Get a normal iterator.
     */
    GOTT_EXPORT iterator get() const;
  
  private:
    class IMPL;
    boost::scoped_ptr<IMPL> const pIMPL;

    tagged_iterator(IMPL *);
    friend class iterator;
  };

  /**
   * Get an iterator for the root node.
   */
  GOTT_EXPORT iterator get_root() const;

private: // Inherited interface
  // Don't call these directly (exported symbols for no use is nothing I like)
  void add_tag(nstring const &);
  void data(xany::Xany const &);
  void begin();
  void end();
  
  pth point();
  void revert(pth const &);
  void get_rid_of(pth const &);

public:
  GOTT_EXPORT void copy_to(writable_structure &ws) const;

  /**
   * Checks whether the tree is equal to another tree.
   * \param o The tree to check against.
   */
  GOTT_EXPORT bool operator==(tree const &o) const;

public:
  GOTT_EXPORT tree();
  GOTT_EXPORT ~tree();

#ifdef DEBUG
  GOTT_EXPORT void dump(std::wostream &);
#endif

private:
  class IMPL;
  boost::scoped_ptr<IMPL> p;

  GOTT_EXPORT friend void intrusive_ptr_add_ref(node *);
  GOTT_EXPORT friend void intrusive_ptr_release(node *);
};

/**
 * Checks whether two trees differ.
 * \param lhs The one tree to check against the other.
 * \param rhs The other tree.
 */
inline bool operator!=(tree const &lhs, tree const &rhs) {
  return !(lhs == rhs);
}

/**
 * Print a tree::iterator to a (wide) stream.
 * \param s The stream to print to.
 * \param i The iterator to print.
 */
template<class Ch>
std::basic_ostream<Ch> &
operator<<(std::basic_ostream<Ch> &s, tree::iterator const &i)
GOTT_EXPORT;

}}}

#endif
