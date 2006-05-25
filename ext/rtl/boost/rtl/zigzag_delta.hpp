// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_ZIGZAG_DELTA_HPP_INCLUDED
#define BOOST_RTL_ZIGZAG_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/intersection.hpp>
#include <boost/rtl/union.hpp>
#include <boost/rtl/difference.hpp>
#include <boost/rtl/isection.hpp>

namespace boost { namespace rel {

	// union
	template<class R> 
    struct traits_impl<R, union_tag>
	{
		typedef typename R::arg1_type tab1;
		typedef typename R::arg2_type tab2;
		typedef typename traits<tab1>::minus_type min1;
		typedef typename traits<tab2>::minus_type min2;
		typedef typename traits<tab1>::plus_type pls1;
		typedef typename traits<tab2>::plus_type pls2;

		//  ((m1+(m2-t1))+(t2*p1))
		typedef typename union_t
		<
			typename union_t
			<
				min1,
				typename diff_t
				<
					min2,
					tab1
				>::type
			>::type,
			typename rsection_t
			<
				pls1,
				tab2
			>::type
		>::type minus_type;

	// ((p1+((t2*m1)-m2))+(p2-(t1-m1)))	
		typedef typename union_t
		<
			typename union_t
			<
				pls1,
				typename difference_t
				<
					typename rsection_t
					<
						min1,
						tab2
					>::type,
					min2
				>::type
			>::type,
			typename diff_t
			<
				pls2,
				typename difference_t
				<
					tab1,
					min1
				>::type
			>::type
		>::type plus_type;
	};

	// intersection
	template<class R> 
    struct traits_impl<R, intersection_tag>
	{
		typedef typename R::arg1_type tab1;
		typedef typename R::arg2_type tab2;
		typedef typename traits<tab1>::minus_type min1;
		typedef typename traits<tab2>::minus_type min2;
		typedef typename traits<tab1>::plus_type pls1;
		typedef typename traits<tab2>::plus_type pls2;

		// ((m1*t2)+(t1*(m2-p2)))
		typedef typename union_t
		<
			typename isection_t
			<
				min1,
				tab2
			>::type,
			typename rsection_t
			<
				typename difference_t
				<
					min2,
					pls2
				>::type,
				tab1
			>::type
		>::type minus_type;
		
	//  (((p1*t2)-m2)+(p1*p2))+(((t1*p2)-m1)-m2))
		typedef typename union_t
		<
			typename union_t
			<
				typename difference_t
				<
					typename isection_t
					<
						pls1,
						tab2
					>::type,
					min2
				>::type,
				typename intersection_t
				<
					pls1,
					pls2
				>::type
			>::type,
			typename difference_t
			<
				typename difference_t
				<
					typename rsection_t
					<
						pls2,
						tab1
					>::type,
					min1
				>::type,
				min2
			>::type
		>::type plus_type;
	};

	// difference
	template<class R> 
    struct traits_impl<R, difference_tag>
	{
		typedef typename R::arg1_type tab1;
		typedef typename R::arg2_type tab2;
		typedef typename traits<tab1>::minus_type min1;
		typedef typename traits<tab2>::minus_type min2;
		typedef typename traits<tab1>::plus_type pls1;
		typedef typename traits<tab2>::plus_type pls2;

		//  ((m1+(t1*p2))-t2)
		typedef typename diff_t
		<
			typename union_t
			<
				min1,
				typename rsection_t
				<
					pls2,
					tab1
				>::type
			>::type,
			tab2
		>::type minus_type;

	//  ((((t1-m1)*m2)+p1)-((t2-m2)+p2))
		typedef typename diff_t
		<
			typename union_t
			<
				typename rsection_t
				<
					min2,
					typename difference_t
					<
						tab1,
						min1
					>::type
				>::type,
				pls1
			>::type,
			typename union_t
			<
				typename difference_t
				<
					tab2,
					min2
				>::type,
				pls2
			>::type
		>::type plus_type;
	};

    // union

	template<class R>
	typename traits<R>::delta_type delta(const R& un, const transaction& tr, union_tag)
	{
        typename traits<typename R::arg1_type>::delta_type d1 = delta(un.argument1(), tr);
		typename traits<typename R::arg1_type>::plus_type p1 = d1.first;
		typename traits<typename R::arg1_type>::minus_type m1 = d1.second;

        typename traits<typename R::arg2_type>::delta_type d2 = delta(un.argument2(), tr);
		typename traits<typename R::arg2_type>::plus_type p2 = d2.first;
		typename traits<typename R::arg2_type>::minus_type m2 = d2.second;

        typename R::arg1_type t1 = un.argument1();
		typename R::arg2_type t2 = un.argument2();

		typename traits<R>::minus_type minus =
			Union(Union(m1, diff(m2, t1)), rsection(p1, t2));

		typename traits<R>::plus_type plus = 
			Union(Union(p1, difference(rsection(m1, t2), m2)), diff(p2, difference(t1, m1)));

        return std::make_pair(plus, minus);
	}

	//intersection

	template<class R>
	typename traits<R>::delta_type delta(const R& un, const transaction& tr, intersection_tag)
	{
        typename traits<typename R::arg1_type>::delta_type d1 = delta(un.argument1(), tr);
		typename traits<typename R::arg1_type>::plus_type p1 = d1.first;
		typename traits<typename R::arg1_type>::minus_type m1 = d1.second;

        typename traits<typename R::arg2_type>::delta_type d2 = delta(un.argument2(), tr);
		typename traits<typename R::arg2_type>::plus_type p2 = d2.first;
		typename traits<typename R::arg2_type>::minus_type m2 = d2.second;

        typename R::arg1_type t1 = un.argument1();
		typename R::arg2_type t2 = un.argument2();

		typename traits<R>::minus_type minus =
			Union(isection(m1, t2), rsection(difference(m2, p2), t1));

		typename traits<R>::plus_type plus = 
			Union(Union(difference(isection(p1, t2), m2),intersection(p1, p2)), difference(difference(rsection(p2, t1), m1), m2));

        return std::make_pair(plus, minus);
	}

	// difference

	template<class R>
	typename traits<R>::delta_type delta(const R& un, const transaction& tr, difference_tag)
	{
        typename traits<typename R::arg1_type>::delta_type d1 = delta(un.argument1(), tr);
		typename traits<typename R::arg1_type>::plus_type p1 = d1.first;
		typename traits<typename R::arg1_type>::minus_type m1 = d1.second;

        typename traits<typename R::arg2_type>::delta_type d2 = delta(un.argument2(), tr);
		typename traits<typename R::arg2_type>::plus_type p2 = d2.first;
		typename traits<typename R::arg2_type>::minus_type m2 = d2.second;

        typename R::arg1_type t1 = un.argument1();
		typename R::arg2_type t2 = un.argument2();

		typename traits<R>::minus_type minus =
			diff(Union(m1, rsection(p2, t1)), t2);

		typename traits<R>::plus_type plus = 
			diff(Union(rsection(m2, difference(t1, m1)), p1),Union(difference(t2, m2), p2));

        return std::make_pair(plus, minus);
	}

}}

#endif //BOOST_RTL_ZIGZAG_DELTA_HPP_INCLUDED
