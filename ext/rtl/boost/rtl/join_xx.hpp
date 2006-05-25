// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_JOIN_XX_HPP_INCLUDED
#define BOOST_RTL_JOIN_XX_HPP_INCLUDED

#include <boost/rtl/range_join.hpp>

namespace boost { namespace rel {

	template<class T1, class T2, class FieldList>
	struct join_eq_t : equal_join_t<T1, T2, FieldList>
	{};

	template<class T1, class T2, class FieldList>
	struct join_lt_t : range_join_t<T1, T2, begin, lower_bound_t<FieldList> >
	{};

	template<class T1, class T2, class FieldList>
	struct join_le_t : range_join_t<T1, T2, begin, upper_bound_t<FieldList> >
	{};

	template<class T1, class T2, class FieldList>
	struct join_gt_t : range_join_t<T1, T2, upper_bound_t<FieldList>, end>
	{};

	template<class T1, class T2, class FieldList>
	struct join_ge_t : range_join_t<T1, T2, lower_bound_t<FieldList>, end>
	{};

	template<class FieldList, class T1, class T2>
		typename join_eq_t<T1, T2, FieldList>::type 
		join_eq(const T1& t1, const T2& t2)
	{
		return equal_join<FieldList>(t1, t2);
	}

	template<class FieldList, class T1, class T2>
		typename join_lt_t<T1, T2, FieldList>::type 
		join_lt(const T1& t1, const T2& t2)
	{
		return range_join(t1, t2, begin(), lower_bound<FieldList>());
	}

	template<class FieldList, class T1, class T2>
		typename join_le_t<T1, T2, FieldList>::type 
		join_le(const T1& t1, const T2& t2)
	{
		return range_join(t1, t2, begin(), upper_bound<FieldList>());
	}

	template<class FieldList, class T1, class T2>
		typename join_gt_t<T1, T2, FieldList>::type 
		join_gt(const T1& t1, const T2& t2)
	{
		return range_join(t1, t2, upper_bound<FieldList>(), end());
	}

	template<class FieldList, class T1, class T2>
		typename join_ge_t<T1, T2, FieldList>::type 
		join_ge(const T1& t1, const T2& t2)
	{
		return range_join(t1, t2, lower_bound<FieldList>(), end());
	}
}}
#endif
