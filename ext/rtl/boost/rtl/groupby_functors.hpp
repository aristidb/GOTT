// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_GROUPBY_FUNCTORS_HPP_INCLUDED
#define BOOST_RTL_GROUPBY_FUNCTORS_HPP_INCLUDED

#include <boost/rtl/tuple.hpp>

namespace boost { namespace rel {

	// sum

	template <class Column> 
	struct sum_t : public column<sum_t<Column>, typename Column::type>
	{
		template<class Tuple> void operator()(const Tuple& t)
		{
			this->value() += BOOST_RTL_FIELD(t, Column);
		}
	};

	template<class Column> 
		struct sum : public column_name<sum_t<Column> > {};

	// counter

	template<class T>
    struct counter_t : public column<counter_t<T>, T>
	{
		counter_t(T i = T()) : column<counter_t<T>, T>(i) {}   // DA to use while comparing against counter
		template<class Tuple> void operator()(const Tuple&)
		{
			++this->value();
		}
	};

	struct counter : public column_name<counter_t<int> > {};

	// min

	template <class Column> 
	class min_t : public column<min_t<Column>, typename Column::type>
	{
	public:
		min_t() : empty_(true){}

		template<class Tuple> void operator()(const Tuple& t)
		{
			if (empty_ || BOOST_RTL_FIELD(t, Column) < this->value())
				this->value() = BOOST_RTL_FIELD(t, Column);

			empty_ = false;
		}
	private:
		bool empty_;
	};

	template<class Column> 
		struct min : public column_name<min_t<Column> > {};

	// max

	template <class Column> 
	class max_t : public column<max_t<Column>, typename Column::type>
	{
	public:
		max_t() : empty_(true){}

		template<class Tuple> void operator()(const Tuple& t)
		{
			if (empty_ || BOOST_RTL_FIELD(t, Column) > this->value())
				this->value() = BOOST_RTL_FIELD(t, Column);

			empty_ = false;
		}
	private:
		bool empty_;
	};

	template<class Column> 
		struct max : public column_name<max_t<Column> > {};
}}

#endif//BOOST_RTL_GROUPBY_FUNCTORS_HPP_INCLUDED
