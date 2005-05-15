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
#ifndef GUARD_EBL_UTIL_TYPE_TRAITS_CONTAINER_TRAITS_HPP_INCLUDED
#define GUARD_EBL_UTIL_TYPE_TRAITS_CONTAINER_TRAITS_HPP_INCLUDED 1

#include "aux/container_reference_traits_default.hpp"
#include "aux/container_const_reference_traits_default.hpp"
#include "aux/container_iterator_traits_default.hpp"
#include "aux/container_const_iterator_traits_default.hpp"
#include "aux/container_value_type_traits_default.hpp"
#include "aux/container_allocator_traits_default.hpp"
#include <stack>

namespace ebl
{
  namespace util
  {
    namespace type_traits
    {
      template < class Container >
	struct ContainerTraits
	{
	  typedef Container type;
	};
      template < class A , class B >
	struct ContainerTraits< std::stack<A,B> >
	{
	  typedef typename std::stack<A,B>::container_type type;
	};

      /**
       * container_traits is used to solve special
       */
      
      template < class ContainerType                                                                         ,
	         template <class> class ReferenceTraits      = aux::container_reference_traits_default       ,
		 template <class> class ConstReferenceTraits = aux::container_const_reference_traits_default ,
		 template <class> class IteratorTraits       = aux::container_iterator_traits_default        ,
		 template <class> class ConstIteratorTraits  = aux::container_const_iterator_traits_default  ,
	 	 template <class> class ValueTypeTraits      = aux::container_value_type_traits_default      ,
		 template <class> class AllocatorTraits      = aux::container_allocator_traits_default
	       >
        struct container_traits
	{
	  typedef typename ContainerTraits<ContainerType>::type        container_type;
	  typedef typename ReferenceTraits<container_type>::type       reference;
	  typedef typename ConstReferenceTraits<container_type>::type  const_reference;
	  typedef typename IteratorTraits<container_type>::type        iterator;
	  typedef typename ConstIteratorTraits<container_type>::type   const_iterator;
	  typedef typename ValueTypeTraits<container_type>::type       value_type;
	  typedef typename AllocatorTraits<container_type>::type       allocator_type;
	};
    }//namespace ebl::util::type_traits
  }//namespace ebl::util
}//namespace ebl

#endif //GUARD_EBL_UTIL_TYPE_TRAITS_CONTAINER_TRAITS_HPP_INCLUDED

