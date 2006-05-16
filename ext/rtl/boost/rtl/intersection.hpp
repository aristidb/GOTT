// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_INTERSECTION_HPP_INCLUDED
#define BOOST_RTL_INTERSECTION_HPP_INCLUDED

#include <boost/rtl/zigzag.hpp>
#include <boost/rtl/keys.hpp>

namespace boost { namespace rel {

	template<class Table1, class Table2>
		struct intersection_t
	{
		typedef zigzag<Table1, Table2, i_pl> type;
	};

	template<class Table1, class Table2>
		typename intersection_t<Table1, Table2>::type intersection(const Table1& t1, const Table2& t2)
	{
		return typename intersection_t<Table1, Table2>::type(t1, t2);
	}
}}

#endif//BOOST_RTL_INTERSECTION_HPP_INCLUDED
