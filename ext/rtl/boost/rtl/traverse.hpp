// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_TRAVERSE_HPP_INCLUDED
#define BOOST_RTL_TRAVERSE_HPP_INCLUDED

namespace boost { namespace rel {

	template<class T, class F> 
		void traverse(T t, F f)
	{
        typename T::tag tag;
		f(t, tag);

		t.traverse_args(f);
	}
}}

#endif//BOOST_RTL_TRAVERSE_HPP_INCLUDED
