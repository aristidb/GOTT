// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_PROJECTION_DELTA_HPP_INCLUDED
#define BOOST_RTL_PROJECTION_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/projection.hpp>
#include <boost/rtl/groupby_delta.hpp>
#include <boost/rtl/groupby_functors.hpp>
#include <boost/rtl/join_xx.hpp>
#include <boost/rtl/isection.hpp>
#include <boost/rtl/difference.hpp>
#include <boost/rtl/isection.hpp>

namespace boost { namespace rel {

	template<class R> 
    struct traits_impl<R, projection_tag>
	{
		// min(proj(t)) = diff(proj(min(t)), proj(difference(t, min(t))); 
		// pls(proj(t)) = diff(proj(pls(t)), proj(difference(t, min(t))); 

		typedef dprj_t
		<
			typename difference_t
			<
				typename R::arg_type,
				typename traits<typename R::arg_type>::minus_type
			>::type,
			typename R::restriction
		> helper_type;

		typedef typename diff_t
		<
			dprj_t
			<
				typename traits<typename R::arg_type>::minus_type, 
				typename R::restriction
			>,
			helper_type
		>::type minus_type;

		typedef typename diff_t
		<
			dprj_t
			<
				typename traits<typename R::arg_type>::plus_type, 
				typename R::restriction
			>,
			helper_type
		>::type plus_type;
	};

    template<class R>
    typename traits<R>::delta_type delta(const R& proj, const transaction& tr, projection_tag)
	{
        typename traits<typename R::arg_type>::delta_type d = 
            delta(proj.argument(), tr);

		typename traits_impl<R, projection_tag>::helper_type helper = 
            difference(proj.argument(), d.second);

		typename traits<R>::minus_type minus = 
            diff(projection<typename R::field_list>(d.second), helper);

		typename traits<R>::plus_type plus = 
            diff(projection<typename R::field_list>(d.first), helper);

        return std::make_pair(plus, minus);
	}

}}

#endif//BOOST_RTL_PROJECTION_DELTA_HPP_INCLUDED
