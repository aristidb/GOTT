// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Properties library
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

#ifndef GOTT_UTIL_PROPERTIES_EXTERNAL_STORAGE_HPP
#define GOTT_UTIL_PROPERTIES_EXTERNAL_STORAGE_HPP

#include <boost/function.hpp>

/**
 * External storage policy. Gets and sets its function via a function/functor.
 * \param T The values' type.
 * \param Input The input functor type.
 * \param Output The output functor type.
 */
template<
  class T, 
  class Input = boost::function<T ()>, 
  class Output = boost::function<void (T)>
>
class external_storage { // (optional) TODO: use thread-local-storage
public:
  /**
   * Constructor.
   * \param in_f The function/functor for getting the value.
   * \param out_f The function/functor for setting the value.
   */
  external_storage(Input in_f = Input(), Output out_f = Output())
  : in(in_f), out(out_f) {}

  T *get_pointer() { return new T(in()); }
  T *get_pointer_noread() { return new T; }
  T const *get_pointer() const { return new T(in()); }
  void finish_pointer(T *p) { out(*p); delete p; }
  void finish_pointer(T const *p) const { delete p; }
  
private:
  Input in;
  Output out;
};

#endif
