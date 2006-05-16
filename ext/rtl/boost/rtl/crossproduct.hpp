// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_CROSSPRODUCT_HPP_INCLUDED
#define BOOST_RTL_CROSSPRODUCT_HPP_INCLUDED

#include <boost/rtl/range_join.hpp>

namespace boost { namespace rel {

	// cross_product_t

	template <class Table1, class Table2>
    struct cross_product_t : range_join_t<Table1, Table2, begin, end>
	{};

	// cross_product

	template <class Table1, class Table2>
		typename cross_product_t<Table1, Table2>::type cross_product(const Table1& t1, const Table2& t2)
	{
		return range_join(t1, t2, begin(), end());
	}
}}

#endif//BOOST_RTL_CROSSPRODUCT_HPP_INCLUDED
