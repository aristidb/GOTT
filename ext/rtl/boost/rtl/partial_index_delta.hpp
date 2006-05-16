// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_PARTIAL_INDEX_DELTA_HPP_INCLUDED
#define BOOST_RTL_PARTIAL_INDEX_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/partial_index.hpp>

namespace boost { namespace rel {

	template<class R> 
    struct traits_impl<R, partial_index_tag>
	{
        typedef typename partial_index_t<typename traits<typename R::arg_type>::plus_type,typename R::strategy_type>::type plus_type;
        typedef typename partial_index_t<typename traits<typename R::arg_type>::minus_type,typename R::strategy_type>::type minus_type;
	};

	template<class R>
    typename traits<R>::delta_type delta(const R& idx, const transaction& tr, partial_index_tag)
	{
        typename traits<typename R::arg_type>::delta_type d = delta(idx.argument(), tr);

        typename traits<R>::plus_type plus(d.first);
        typename traits<R>::minus_type minus(d.second);

        return std::make_pair(plus, minus);
	}

}}

#endif
