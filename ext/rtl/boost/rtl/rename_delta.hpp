// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_RENAME_DELTA_HPP_INCLUDED
#define BOOST_RTL_RENAME_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/rename.hpp>

namespace boost { namespace rel {

	template<class R> 
    struct traits_impl<R, conversion_tag>
	{
		typedef cnv_t<typename traits<typename R::arg_type>::plus_type, typename R::pairs> plus_type;
		typedef cnv_t<typename traits<typename R::arg_type>::minus_type, typename R::pairs> minus_type;
	};

	template<class R>
	typename traits<R>::delta_type delta(const R& cnv, const transaction& tr, conversion_tag)
	{
        typename traits<R>::delta_type d(delta(cnv.argument(), tr));
		typename traits<R>::plus_type plus(d.first);
		typename traits<R>::minus_type minus(d.second);
        return std::make_pair(plus,minus);
	};

}}

#endif//BOOST_RTL_RENAME_DELTA_HPP_INCLUDED 
