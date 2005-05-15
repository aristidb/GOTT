/*
* Copyright 2005 Vinzenz Feenstra - evilissimo@users.sourceforge.net
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/ 
#ifndef GUARD_EBL_UTIL_TYPE_TRAITS_IS_SET_HPP_INCLUDED
#define GUARD_EBL_UTIL_TYPE_TRAITS_IS_SET_HPP_INCLUDED 1

#include <set>

namespace ebl
{
  namespace util
  {
    namespace type_traits
    {
      /**
       *  @brief is_set::value is true if Type is a std::set or a std::multiset
       *         is_set::multi is true if Type is a std::multiset
       */
      template < class Type >
        struct is_set
        {
          enum { value = false, 
		 multi = false 
	       };
        };
      /**
       *  is_set specialisation for std::set
       */
      template <class A,class B, class C>
	struct is_set< std::set<A,B,C> >
	{
	  enum { value = true, 
		 multi = false 
	       };
	};
      /**
       *  is_set specialisation for std::multi_set
       */
      template <class A,class B, class C>
	struct is_set<std::multiset<A,B,C> >
	{
	  enum { value = true, 
		 multi = true
	       };
	};
    }//namespace ebl::util::type_traits
  }//namespace ebl::util
}//namespace ebl

#endif //GUARD_EBL_UTILS_TYPE_TRAITS_IS_SET_HPP_INCLUDED

