// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_IDENTIFIABLE_HPP_INCLUDED
#define BOOST_RTL_IDENTIFIABLE_HPP_INCLUDED

#include <boost/smart_ptr.hpp>
#include <cstddef>

namespace boost { namespace rel {

	class identifiable
	{
	public:
		identifiable()
			: p(new char)
		{}
		ptrdiff_t id() const
		{
			return p.get() - (char*)0;
		}
	private:
		boost::shared_ptr<char> p;
	};

	struct id_less
	{
		template<class Ptr>
			bool operator()(Ptr p1, Ptr p2) const
		{
			return p1->id() < p2->id();
		}
	};

}}

#endif//BOOST_RTL_IDENTIFIABLE_HPP_INCLUDED
