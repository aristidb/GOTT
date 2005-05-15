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
#ifndef GUARD_EBL_VIEW_ARRAY_HPP_INCLUDED
#define GUARD_EBL_VIEW_ARRAY_HPP_INCLUDED 1

#include <ebl/view/aux/fast_array_view_impl.hpp>

namespace ebl
{
  namespace view
  {
    /**
     * @brief General array_view interface
     */
    template < class Container,
	       template<class> class Implementation = ebl::view::aux::array_view_impl
	     >
      struct array_view
      {
        typedef typename ebl::view::aux::
		array_view_traits< Container >::value_type        value_type;
	typedef typename ebl::view::aux:: 
		array_view_traits< Container >::reference         reference;
	typedef typename ebl::view::aux::
		array_view_traits< Container >::const_reference   const_reference;
	typedef typename ebl::view::aux::
		array_view_traits< Container >::iterator          iterator;
	typedef typename ebl::view::aux::
		array_view_traits< Container >::const_iterator    const_iterator;
        typedef typename ebl::view::aux::
		array_view_traits< Container >::container_type    container_type;
	
         
	explicit array_view(container_type & c)
	 : m_cont(c),avt_f(c)
	{
	}
        
	reference  operator[](std::size_t idx)
	{
	  return  avt_f(idx);
	}
	const_reference operator[](std::size_t idx) const
        {
	  return avt_f(idx);
	}
        private: // a reference to the container
	  container_type & m_cont;
	
	private: // implementation of the operator[] as functor

	typedef Implementation<Container> array_view_impl;
	
	array_view_impl avt_f;
	
	private:
	//hiding default C'tor
	array_view();
      };
  }
}

#endif //GUARD_EBL_VIEW_ARRAY_HPP_INCLUDED

