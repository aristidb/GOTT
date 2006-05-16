// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_TRAITS_HPP_INCLUDED
#define BOOST_RTL_TRAITS_HPP_INCLUDED

#include <boost/rtl/union.hpp>
#include <boost/rtl/difference.hpp>

namespace boost { namespace rel {

    template<class R, class Tag = typename R::tag> 
    struct traits_impl;

	template<class R> 
    struct traits
	{
		typedef typename traits_impl<R>::plus_type plus_type;
		typedef typename traits_impl<R>::minus_type minus_type;
		typedef std::pair<plus_type, minus_type> delta_type;
		
        typedef typename union_t<
            typename difference_t<R, minus_type>::type, 
            plus_type
        >::type modified_type;
	};

	class transaction;

	template<class R>
	typename traits<R>::delta_type delta(const R& r, const transaction& tr)
	{
        typename R::tag tag;
        return delta(r, tr, tag);
	}

	template<class R>
	typename traits<R>::modified_type modified(const R& r, const transaction& tr)
	{
        typename traits<R>::delta_type d = delta(r, tr);
		return Union(difference(r, d.second), d.first);
	}

}};

#endif//BOOST_RTL_TRAITS_HPP_INCLUDED

