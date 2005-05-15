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
#ifndef GUARD_EBL_VIEW_AUX_ARRAY_VIEW_IMPL_HPP_INCLUDED
#define GUARD_EBL_VIEW_AUX_ARRAY_VIEW_IMPL_HPP_INCLUDED 1

//#include <boost/mpl/if.hpp>

#include <ebl/util/type_traits/is_set.hpp>
#include <ebl/view/aux/array_view_traits.hpp>

#include <iostream>
#include <set>
#include <vector>	
#include <algorithm>

namespace ebl
{
  namespace view
  {
    namespace aux
    {
    
      /** @brief array_view_impl is the implementation for the "ArrayView"
       *   template parameter description:
       *     Type      - the value_type contained in the Container
       *     Container - the container type 
       *     Dim       - basic size of the Index Vector
       *     Diff      - difference size between the index positions
       *
       *  this template class is used by array_view as implementation 
       *  for the operator[]
       */
      template < 
	         class Container,	 
	         int Dim  = 256, 
		 int Diff = 16
	       >
	struct array_view_impl
	{
          typedef typename array_view_traits< Container >::reference        reference;
          typedef typename array_view_traits< Container >::const_reference  const_reference;
          typedef typename array_view_traits< Container >::iterator         iterator;
          typedef typename array_view_traits< Container >::const_iterator   const_iterator;
	  typedef typename array_view_traits< Container >::container_type   container_type;
	  typedef typename array_view_traits< Container >::value_type       value_type;
	  
          array_view_impl(container_type & c ) 
	  : cont(c), 
	    size(c.size())
	  {
	    m_index.reserve(Dim);
	    
	    // building index
	    unsigned rest = size % Diff;
	    unsigned rep  = size / Diff;

	    iterator sb = cont.begin();
	    
	    while(rep--)
	    {
	      m_index.push_back(sb);
	      std::advance(sb,Diff);
	    }
            
	    if(rest > 0)
  	      m_index.push_back(sb);
	  }
	  
	  reference operator()( std::size_t idx )
	  {
	    // using the index to access the elements
            iterator it = m_index[idx / Diff];	   
	    std::advance(it,idx % Diff);
	    return *it;
	  }
	  const_reference operator()( std::size_t idx ) const
	  {
            const_iterator it = m_index[idx / Diff];
	    std::advance(it,idx % Diff);
	    return *it;
	  }
	  private:
	  container_type & cont;	  
	  std::vector< iterator > m_index;
	  std::size_t size;
	};
      
      /** @brief array_view_impl specialization for std::vector 
       *         ( is as fast as std::vector itself )
       */
      template < 
	         class Type 
	       >
        struct array_view_impl< std::vector<Type> >
	{
	   typedef typename std::vector<Type>::reference reference;
	   typedef typename std::vector<Type>::const_reference const_reference;
	   
	   array_view_impl( std::vector<Type> & c )
	    : cont(c)
	   {
	   }
	   reference operator()(std::size_t idx)
	   {
	     return cont[idx];
	   }
	   const_reference operator()(std::size_t idx ) const
	   { 
	     return cont[idx];
	   }
	   std::vector<Type> & cont;
	};
    }//namespace ebl::view::aux
  }//namespace ebl::view
}//namespace ebl

#endif //GUARD_EBL_VIEW_AUX_ARRAY_VIEW_IMPL_HPP_INCLUDED
 
