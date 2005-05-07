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

template<class Iterator> class selection {
public:
  selection() {}
  explicit selection(Iterator const &i) : impl(1, i) {}
  template<class I> selection(I a, I b) : impl(a, b) {}

  selection const &operator+=(selection const &) EXPORT;

  selection find(std::size_t i) const EXPORT;
  selection find_range(std::size_t start, std::size_t stop) const EXPORT;
  selection find_all() const EXPORT;
  selection find_tag(std::wstring const &) const EXPORT;

private:
  typedef std::vector<Iterator> container;
public:
  typedef typename container::iterator iterator;
  typedef typename container::const_iterator const_iterator;

  const_iterator begin() const { return impl.begin(); }
  const_iterator end() const { return impl.end(); }

  bool operator==(selection const &);

private:
  container impl;

  void add(Iterator const &);
  void add_windex(Iterator const &, std::size_t);
  void add_wrange(Iterator const &, std::size_t, std::size_t);
  void add_wtag(Iterator const &, std::wstring const &);
};

template<class I>
bool operator!=(selection<I> const &a, selection<I> const &b) {
  return !(a == b);
}

}}}}

#endif
