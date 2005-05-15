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
#ifndef GUARD_EBL_UTIL_TYPE_TRAITS_AUX_CONTAINER_ALLOCATOR_TRAITS_DEFAULT_HPP_INCLUDED
#define GUARD_EBL_UTIL_TYPE_TRAITS_AUX_CONTAINER_ALLOCATOR_TRAITS_DEFAULT_HPP_INCLUDED 1

namespace ebl
{
  namespace util
  {
    namespace type_traits
    {
      namespace aux
      {
        template < class Container >
          struct container_allocator_traits_default
  	  {
	    typedef typename Container::allocator_type type;
	  };
      }//namespace ebl::util::type_traits::aux
    }//namespace ebl::util::type_traits
  }//namespace ebl::util
}//namespace ebl

#endif //GUARD_EBL_UTIL_TYPE_TRAITS_AUX_CONTAINER_ALLOCATOR_TRAITS_DEFAULT_HPP_INCLUDED 
