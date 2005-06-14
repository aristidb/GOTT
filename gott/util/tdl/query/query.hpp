// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL query engine
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

#ifndef GOTT_UTIL_TDL_QUERY_TDLQUERY_HPP
#define GOTT_UTIL_TDL_QUERY_TDLQUERY_HPP

#include <gott/util/misc/commonheaders.hpp>

namespace gott {
namespace util {
namespace tdl {
namespace query {

/**
 * A set of iterators of a structure::tree::iterator compatible type.
 */
template<class Iterator> class selection {
public:
  /// Create an empty selection.
  selection() {}
  
  /**
   * Create a selection with a single element.
   * \param i The selection's content.
   */
  explicit selection(Iterator const &i) : impl(1, i) {}

  /**
   * Create a selection from a STL-compatible iterator range.
   * \param a An iterator to the beginning of the range.
   * \param b An iterator just after the end of the range.
   */
  template<class I> selection(I a, I b) : impl(a, b) {}

  /**
   * Add another selection to this selection.
   * \param o The selection to add.
   */
  selection const &operator+=(selection const &o) EXPORT;

  /**
   * From every iterator, get an index-specific child.
   * \param i The children's index.
   * \return The new selection with the children.
   */
  selection find(std::size_t i) const EXPORT;

  /**
   * From every iterator, get a range of index-specific children.
   * \param start The children's minimal (inclusive) index.
   * \param stop The children's maximal (inclusive) index.
   * \return The new selection with the children.
   */
  selection find_range(std::size_t start, std::size_t stop) const EXPORT;

  /**
   * Gets all children from the iterators.
   * \return The new selection with the children.
   */
  selection find_all() const EXPORT;

  /** 
   * Gets all children from the iterators with a specific tag.
   * \param tag The tag to search for.
   */
  selection find_tag(std::wstring const &tag) const EXPORT;

private:
  typedef std::vector<Iterator> container;
public:
  typedef typename container::iterator iterator;
  typedef typename container::const_iterator const_iterator;
  typedef typename container::size_type size_type;

  const_iterator begin() const { return impl.begin(); }
  const_iterator end() const { return impl.end(); }
  size_type size() const { return impl.size(); }

  /// Compare two selections for equality.
  bool operator==(selection const &) const EXPORT;

private:
  container impl;

  void add(Iterator const &);
  void add_windex(Iterator const &, std::size_t);
  void add_wrange(Iterator const &, std::size_t, std::size_t);
  void add_wtag(Iterator const &, std::wstring const &);
};

/// Checks whether two selections differ.
template<class I>
bool operator!=(selection<I> const &a, selection<I> const &b) {
  return !(a == b);
}

/// Prints a selection to a stream.
template<class Ch, class ChT, class I>
std::basic_ostream<Ch, ChT> &operator<<(std::basic_ostream<Ch, ChT> &s,
                                        selection<I> const &x) {
  return s << range(x);
}

}}}}

#endif
