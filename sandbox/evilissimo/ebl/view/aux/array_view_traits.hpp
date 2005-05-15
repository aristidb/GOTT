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
#ifndef GUARD_EBL_VIEW_AUX_ARRAY_VIEW_TRAITS_HPP_INCLUDED
#define GUARD_EBL_VIEW_AUX_ARRAY_VIEW_TRAITS_HPP_INCLUDED 1

//#include "../../util/type_traits/is_set.hpp"

namespace ebl
{
  namespace view
  {
    namespace aux
    {
      template < class Container > 
	struct array_view_traits
	{
           template < class ContType , bool Cond = false>
             struct set_traits
	     {
	       typedef typename ContType::reference reference;
	     };
      
           template < class ContType >
	     struct set_traits<ContType,true>
	     {
	       typedef typename ContType::key_type const & reference;
  	     }; 

	   typedef typename set_traits< Container ,                
		                        ebl::util::type_traits::is_set< Container >::value
				      >::reference             reference;
	   typedef typename Container::const_reference         const_reference;
	   typedef typename Container::iterator                iterator;
	   typedef typename Container::const_iterator          const_iterator;
	   typedef Container                                   container_type;
	   typedef typename Container::value_type              value_type;
	};
    }//namespace ebl::view::aux
  }//namespace ebl::view
}//namespace ebl

#endif // GUARD_EBL_VIEW_AUX_ARRAY_VIEW_TRAIS_HPP_INCLUDED
