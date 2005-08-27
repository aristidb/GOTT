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
#ifndef GUARD_EBL_VIEW_DYNAMIC_CONST_ARRAY_VIEW_HPP_INCLUDED
#define GUARD_EBL_VIEW_DYNAMIC_CONST_ARRAY_VIEW_HPP_INCLUDED 1

namespace ebl
{
  namespace view
  {
    template < class Container ,
	       class Traits   
	     >
      struct dynamic_const_array_view
      {
        typedef typename Traits::const_reference const_reference;
	virtual ~dynamic_array_view(){}
	virtual const_reference operator[](size_t idx) const = 0;
      };
  }//namespace ebl::view
}//namespace ebl

#endif//GUARD_EBL_VIEW_DYNAMIC_CONST_ARRAY_VIEW_HPP_INCLUDED


