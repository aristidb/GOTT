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

#include "query.hpp"

using std::wstring;
using std::size_t;
using gott::util::tdl::query::selection;

template<class I>
void selection<I>::add(I const &x) {
  impl.reserve(impl.size() + x.size());
  for (I i = x.first_child(); i; i = i.next())
    impl.push_back(i);
}

template<class I>
void selection<I>::add_windex(I const &x, size_t i) {
  if (x.size() > i)
    impl.push_back(x[i]);
}

template<class I>
void selection<I>::add_wrange(I const &x, size_t a, size_t b) {
  size_t end = std::min(b, x.size());
  
  impl.reserve(impl.size() + (end - a));
  for (; a < end; ++a)
    impl.push_back(x[a]);
}

template<class I>
void selection<I>::add_wtag(I const &x, wstring const &s) {
  for (typename I::tagged i = x.with_tag(s); i; ++i)
    impl.push_back(i.get());
}

template<class I> 
selection<I> const &selection<I>::operator+=(selection const &b) {
  impl.insert(impl.end(), b.impl.begin(), b.impl.end());
  return *this;
}

template<class I>
selection<I> selection<I>::find(size_t i) const {
  selection tmp;
  for (const_iterator it = impl.begin(); it != impl.end(); ++it)
    tmp.add_windex(*it, i);
  return tmp;
}

template<class I>
selection<I> selection<I>::find_range(size_t a, size_t b) const {
  selection tmp;
  for (const_iterator it = impl.begin(); it != impl.end(); ++it)
    tmp.add_wrange(*it, a, b);
  return tmp;
}

template<class I>
selection<I> selection<I>::find_all() const {
  selection tmp;
  for (const_iterator it = impl.begin(); it != impl.end(); ++it)
    tmp.add(*it);
  return tmp;
}

template<class I>
selection<I> selection<I>::find_tag(wstring const &s) const {
  selection tmp;
  for (const_iterator it = impl.begin(); it != impl.end(); ++it)
    tmp.add_wtag(*it, s);
  return tmp;
}

#include "spec.tpp"
