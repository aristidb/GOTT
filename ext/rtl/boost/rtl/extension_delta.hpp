// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_EXTENSION_DELTA_HPP_INCLUDED
#define BOOST_RTL_EXTENSION_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/extension.hpp>

namespace boost { namespace rel {

	template<class R> 
	struct traits_impl<R, extension_tag>
	{
        typedef typename extension_t<typename traits<typename R::arg_type>::plus_type, typename R::policy_type>::type plus_type;
        typedef typename extension_t<typename traits<typename R::arg_type>::minus_type, typename R::policy_type>::type minus_type;
	};

    template<class R> 
    typename traits<R>::delta_type delta(const R& ext, const transaction& tr, extension_tag)
	{
        typename traits<typename R::arg_type>::delta_type d = delta(ext.argument(), tr);

        typename traits<R>::plus_type plus = extension(d.first, ext.policy());
        typename traits<R>::minus_type minus = extension(d.second, ext.policy());

        return std::make_pair(plus, minus);
	}

}}

#endif//BOOST_RTL_EXTENSION_DELTA_HPP_INCLUDED
