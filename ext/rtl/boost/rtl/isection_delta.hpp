// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_ISECTION_DELTA_HPP_INCLUDED
#define BOOST_RTL_ISECTION_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/isection.hpp>

namespace boost { namespace rel {

	template<class R> 
    struct traits_impl<R, isection_tag>
	{
		typedef typename R::arg1_type tab1;
		typedef typename R::arg2_type tab2;
		typedef typename traits<tab1>::minus_type min1;
		typedef typename traits<tab2>::minus_type min2;
		typedef typename traits<tab1>::plus_type pls1;
		typedef typename traits<tab2>::plus_type pls2;

	//  ((m1*t2)+(t1*(m2-p2)))
		typedef typename union_t
		<
			typename isection_t
			<
				min1,
				tab2
			>::type,
			typename rsection_t
			<
				typename diff_t
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

	template<class R> 
    struct traits_impl<R, rsection_tag>
	{
		typedef typename R::arg1_type tab1;
		typedef typename R::arg2_type tab2;
		typedef typename traits<typename R::arg1_type>::minus_type min1;
		typedef typename traits<typename R::arg2_type>::minus_type min2;
		typedef typename traits<typename R::arg1_type>::plus_type pls1;
		typedef typename traits<typename R::arg2_type>::plus_type pls2;

		typedef typename union_t
		<
			typename intersection_t
			<
				min2,
				typename R::arg1_type
			>::type,
			typename intersection_t
			<
				typename R::arg2_type,
				typename difference_t<min1,pls1>::type
			>::type
		>::type minus_type;

		typedef typename union_t
		<
			typename intersection_t<
				pls2,
				typename union_t
				<
					typename difference_t
					<
						typename R::arg1_type,
						min1
					>::type,
					pls1
				>::type
			>::type,
			typename difference_t
			<
				typename difference_t
				<
					typename intersection_t
					<
						typename R::arg2_type,
						pls1
					>::type,
					min2
				>::type,
				min1
			>::type
		>::type plus_type;
	};

	template<class R> 
    struct traits_impl<R, diff_tag>
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

	template<class R>
	typename traits<R>::delta_type
	delta(const R& is, const transaction& tr, isection_tag)
	{
        typename traits<typename R::arg1_type>::delta_type d1 = delta(is.argument1(), tr);  
		typename traits<typename R::arg1_type>::plus_type p1 = d1.first;
		typename traits<typename R::arg1_type>::minus_type m1 = d1.second;

        typename traits<typename R::arg2_type>::delta_type d2 = delta(is.argument2(), tr);  
		typename traits<typename R::arg2_type>::plus_type p2 = d2.first;
		typename traits<typename R::arg2_type>::minus_type m2 = d2.second;

		typename R::arg1_type t1 = is.argument1();
		typename R::arg2_type t2 = is.argument2();

		typename traits<R>::minus_type minus =
			Union(isection(m1,t2),rsection(diff(m2,p2),t1));

		typename traits<R>::plus_type plus = 
			Union(Union(difference(isection(p1,t2),m2),intersection(p1,p2)),difference(difference(rsection(p2,t1),m1),m2));

        return std::make_pair(plus, minus);
	}

	template<class R>
	typename traits<R>::delta_type
	delta(const R& is, const transaction& tr, rsection_tag)
	{
        typename traits<typename R::arg1_type>::delta_type d1 = delta(is.argument1(), tr);  
		typename traits<typename R::arg1_type>::plus_type p1 = d1.first;
		typename traits<typename R::arg1_type>::minus_type m1 = d1.second;

        typename traits<typename R::arg2_type>::delta_type d2 = delta(is.argument2(), tr);  
		typename traits<typename R::arg2_type>::plus_type p2 = d2.first;
		typename traits<typename R::arg2_type>::minus_type m2 = d2.second;

		typename R::arg1_type t1 = is.argument1();
		typename R::arg2_type t2 = is.argument2();

		typename traits<R>::minus_type minus = 
		Union
		(
			intersection(m2,t1),
			intersection
			(
				t2,
				difference(m1,p1)
			)
		);

		typename traits<R>::plus_type plus =
		Union
		(
			intersection
			(
				p2,
				Union
				(
					difference
					(
						t1,
						m1
					),
					p1
				)
			),
			difference
			(
				difference
				(
					intersection
					(
						t2,
						p1
					),
					m2
				),
				m1
			)
		);

        return std::make_pair(plus, minus);
	}

	template<class R>
	typename traits<R>::delta_type
	delta(const R& is, const transaction& tr, diff_tag)
	{
        typename traits<typename R::arg1_type>::delta_type d1 = delta(is.argument1(), tr);  
		typename traits<typename R::arg1_type>::plus_type p1 = d1.first;
		typename traits<typename R::arg1_type>::minus_type m1 = d1.second;

        typename traits<typename R::arg2_type>::delta_type d2 = delta(is.argument2(), tr);  
		typename traits<typename R::arg2_type>::plus_type p2 = d2.first;
		typename traits<typename R::arg2_type>::minus_type m2 = d2.second;

		typename R::arg1_type t1 = is.argument1();
		typename R::arg2_type t2 = is.argument2();

		typename traits<R>::minus_type minus =
			diff(Union(m1,rsection(p2,t1)),t2);

		typename traits<R>::plus_type plus =
			diff(Union(rsection(m2,difference(t1,m1)),p1),Union(difference(t2,m2),p2));

        return std::make_pair(plus, minus);
	}

}}

#endif//BOOST_RTL_ISECTION_DELTA_HPP_INCLUDED
