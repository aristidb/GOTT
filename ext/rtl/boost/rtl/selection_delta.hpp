// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_SELECTION_DELTA_HPP_INCLUDED
#define BOOST_RTL_SELECTION_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/selection.hpp>

namespace boost { namespace rel {

	template<class R>
    struct traits_impl<R, selection_tag>
	{
        typedef typename selection_t<typename traits<typename R::arg_type>::plus_type, typename R::pred_type>::type plus_type;
        typedef typename selection_t<typename traits<typename R::arg_type>::minus_type, typename R::pred_type>::type minus_type;
	};

	template<class R>
    typename traits<R>::delta_type delta(const R& sel, const transaction& tr, selection_tag)
	{
        typename traits<typename R::arg_type>::delta_type d = delta(sel.argument(), tr); 

        typename traits<R>::plus_type plus = selection(d.first, sel.pred());
        typename traits<R>::minus_type minus = selection(d.second, sel.pred());

        return std::make_pair(plus, minus);
	}
}}

#endif//BOOST_RTL_SELECTION_DELTA_HPP_INCLUDED
