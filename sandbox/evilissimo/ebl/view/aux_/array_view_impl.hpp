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

#include <iostream>

#include <vector>	
#include <algorithm>

namespace ebl
{
  namespace view
  {
    namespace aux
    {
      template < class Type , template <class> class Container >
	struct array_view_impl
	{
	  typedef typename Container<Type>::reference       reference; 
	  typedef typename Container<Type>::const_reference const_reference;

          array_view_impl(Container<Type> & c ) 
	  : cont(c)
	  {
	  }
	  
	  reference operator()( std::size_t idx )
	  {
	    typename Container<Type>::iterator it = cont.begin();
	    std::advance(it,idx);
	    return *it;
	  }
	  const_reference operator()( std::size_t idx ) const
	  {
            typename Container<Type>::const_iterator it = cont.begin();
	    std::advance(it,idx);
	    return *it;
	  }
	  private:
	  Container<Type> & cont;	  
	};
      
      template < class Type >
        struct array_view_impl<Type, std::vector >
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
    }
  }
}//namespace ebl

#endif //GUARD_EBL_VIEW_AUX_ARRAY_VIEW_IMPL_HPP_INCLUDED
 
