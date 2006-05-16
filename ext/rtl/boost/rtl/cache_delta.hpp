// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_CACHE_DELTA_HPP_INCLUDED
#define BOOST_RTL_CACHE_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/cache.hpp>

namespace boost { namespace rel {

	template<class R> 
    struct traits_impl<R, cache_tag>
	{
        typedef typename cache_t<typename traits<typename R::arg_type>::plus_type>::type plus_type;
        typedef typename cache_t<typename traits<typename R::arg_type>::minus_type>::type minus_type;
	};

	template<class R>
	typename traits<R>::delta_type delta(const R& ch, const transaction& tr, cache_tag)
	{
        typename traits<typename R::arg_type>::delta_type d = delta(ch.argument(), tr);
        return std::make_pair(cache(d.first), cache(d.second));
	};

}}
#endif
