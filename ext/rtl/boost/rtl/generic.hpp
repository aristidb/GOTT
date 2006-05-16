// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_GENERIC_HPP_INCLUDED
#define BOOST_RTL_GENERIC_HPP_INCLUDED

// This file lists all headers we need from STL and BOOST 
// for core library

// STL

#include <cassert>

// BOOST

#include <boost/config.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/smart_ptr.hpp>

namespace boost { namespace rel {

	template<bool x, bool y> 
		struct static_and
	{
		BOOST_STATIC_CONSTANT(bool, value = (!x) ? false : y);
	};

	template<bool x, bool y> 
		struct static_or
	{
		BOOST_STATIC_CONSTANT(bool, value = x ? true : y);
	};

	struct deleter
	{
		template<class T> 
			void operator()(T* p) const
		{
			delete p;
		}
	};

	struct null_type;
}}

#endif//BOOST_RTL_GENERIC_HPP_INCLUDED
