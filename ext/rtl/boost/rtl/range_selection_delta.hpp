// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_RANGE_SELECTION_DELTA_HPP_INCLUDED
#define BOOST_RTL_RANGE_SELECTION_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/range_selection.hpp>

namespace boost { namespace rel {

	template<class R> 
    struct traits_impl<R, range_selection_tag>
	{
		typedef typename range_selection_t<
			typename traits<typename R::arg_type>::plus_type,
			typename R::begin_type,
			typename R::end_type
        >::type plus_type;

		typedef typename range_selection_t<
			typename traits<typename R::arg_type>::minus_type,
			typename R::begin_type,
			typename R::end_type
        >::type minus_type;
	};

	template<class R>
    typename traits<R>::delta_type delta(const R& rs, const transaction& tr, range_selection_tag)
	{
        typename traits<typename R::arg_type>::delta_type d = delta(rs.argument(), tr);

        typename traits<R>::plus_type plus = range_selection(d.first, rs.begin_functor(), rs.end_functor());
        typename traits<R>::minus_type minus = range_selection(d.second, rs.begin_functor(), rs.end_functor());

        return std::make_pair(plus, minus);
	}

	template<class R> 
	struct traits_impl<R, equal_selection_tag>
	{
		typedef typename equal_selection_t<
			typename traits<typename R::arg_type>::plus_type,
			typename R::sub_type
        >::type plus_type;

		typedef typename equal_selection_t<
			typename traits<typename R::arg_type>::minus_type,
			typename R::sub_type
        >::type minus_type;
	};

	template<class R>
    typename traits<R>::delta_type delta(const R& es, const transaction& tr, equal_selection_tag)
    {
        typename traits<typename R::arg_type>::delta_type d = delta(es.argument(), tr);

        typename traits<R>::plus_type plus = equal_selection(d.first, es.sub());
        typename traits<R>::minus_type minus = equal_selection(d.second, es.sub());

        return std::make_pair(plus, minus);
	}
}}

#endif//BOOST_RTL_RANGE_SELECTION_DELTA_HPP_INCLUDED
