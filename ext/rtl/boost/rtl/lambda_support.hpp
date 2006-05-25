#ifndef BOOST_RTL_LAMBDA_SUPPORT_HPP_INCLUDED
#define BOOST_RTL_LAMBDA_SUPPORT_HPP_INCLUDED

#include <boost/lambda/lambda.hpp>

namespace boost { namespace lambda {

	template<class T, class Column> 
    struct plain_return_type_2<other_action<subscript_action>, T, Column> 
	{
		typedef typename Column::type type;
	};
}}

#endif//BOOST_RTL_LAMBDA_SUPPORT_HPP_INCLUDED
