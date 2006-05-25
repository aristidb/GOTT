// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_RANGE_JOIN_DELTA_HPP_INCLUDED
#define BOOST_RTL_RANGE_JOIN_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/range_join.hpp>

#include <boost/rtl/table.hpp>

namespace boost { namespace rel {

	template<class R> 
    struct traits_impl<R, range_join_tag>
	{
        typedef typename traits<typename R::arg1_type>::delta_type del1;
        typedef typename traits<typename R::arg2_type>::delta_type del2;

		typedef typename traits<typename R::arg1_type>::minus_type min1; 
		typedef typename traits<typename R::arg2_type>::minus_type min2; 
		typedef typename traits<typename R::arg1_type>::plus_type pls1; 
		typedef typename traits<typename R::arg2_type>::plus_type pls2; 
		typedef typename R::arg1_type arg1;
		typedef typename R::arg2_type arg2;
		typedef typename R::begin_type begin_type;
		typedef typename R::end_type end_type;

        typedef typename table_type<R>::type minus_type;
        typedef typename table_type<R>::type plus_type;
	};

	template<class R>
    typename traits<R>::delta_type delta(const R& rj, const transaction& tr, range_join_tag)
	{
		typedef traits_impl<R, range_join_tag> traits_type;

        typename traits_type::del1 d1 = delta(rj.argument1(), tr);
        typename traits_type::del2 d2 = delta(rj.argument2(), tr);

		typename traits_type::min1 m1 = d1.second;
		typename traits_type::min2 m2 = d2.second;
		typename traits_type::pls1 p1 = d1.first;
		typename traits_type::pls2 p2 = d2.first;
        typename R::arg1_type t1 = rj.argument1();
        typename R::arg2_type t2 = rj.argument2();
        typename R::begin_type fbegin = rj.begin_functor();
        typename R::end_type fend = rj.end_functor();

		typename traits_type::minus_type minus;
        minus.append(Union(range_join(m1, t2, fbegin, fend), range_join(difference(t1, m1), m2, fbegin, fend)));

		typename traits_type::plus_type plus;
        plus.append(Union(range_join(p1, difference(t2, m2), fbegin, fend), range_join(Union(difference(t1, m1), p1), p2, fbegin, fend)));

		return std::make_pair(plus, minus);
	}

}}

#endif//BOOST_RTL_RANGE_JOIN_DELTA_HPP_INCLUDED
