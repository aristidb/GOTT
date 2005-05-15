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
#ifndef GUARD_EBL_VIEW_STRING_VIEW_INCLUDED 
#define GUARD_EBL_VIEW_STRING_VIEW_INCLUDED 1

#include "../util/type_traits/container_traits.hpp"
#include <sstream>
#include <iostream>
/**
 * @Todo: Clean up and oder file 
 */
namespace ebl
{
  namespace view
  {
    template < class StringType = std::wstring >
      struct StringStreamTraits
      {
        typedef std::wostringstream type;
	enum { isWString = 1 };
      };
    template <>
      struct StringStreamTraits<std::string>
      {
        typedef std::ostringstream type;
	enum { isWString = 0 };
      };

    template < bool isWide = false>
      struct string_helper
      {
	typedef char sign_type;
        static std::string getEmpty(){ std::cout << "STRING" << std::endl; return std::string(""); }
      };

    template <>
      struct string_helper<true>
      {
	typedef wchar_t  sign_type;
	static std::wstring getEmpty(){ std::wcout << "WSTRING" << std::endl; return std::wstring(L"");}
      };
	
    template < class StringType ,
	       class ContainerType
	     >
      struct string_view
      {
	typedef ebl::util::type_traits::container_traits<ContainerType> traits; 
        typedef typename traits::reference       reference;
	typedef typename traits::const_reference const_reference;
	typedef typename traits::iterator        iterator;
	typedef typename traits::const_iterator  const_iterator;
	typedef typename traits::value_type      value_type;
	typedef ContainerType                    container_type;
    
	typedef string_helper< StringStreamTraits<StringType>::isWString > string_helper;
	
	typedef typename string_helper::sign_type sign_type;
        
	
        string_view( container_type & cont , sign_type sep_sign = 0 )
	{
	  typename StringStreamTraits<StringType>::type os;
	  
	  const_iterator start = cont.begin() , 
			 end   = cont.end(), 
			 tmp   = start;

	  if(sep_sign == 0)
            sep_sign = string_helper::getEmpty()[0];
	  
	  while(start != end)
            os << *(start++) 
	       <<  ( ++tmp != end ? StringType(&sep_sign,1) : string_helper::getEmpty() );
	  
	  m_data = os.str();
	}
	~string_view()
	{
	  
	}
        StringType get(){ return m_data; }
	
	private:

	StringType m_data;
      };
  }//namespace ebl::view
}//namespace ebl

#endif //GUARD_EBL_VIEW_STRING_VIEW_INCLUDED

