// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_UNARY_HPP_INCLUDED
#define BOOST_RTL_UNARY_HPP_INCLUDED

#include <boost/rtl/tuple.hpp>
#include <boost/rtl/iterator_root.hpp>

#define BOOST_RTL_PULL_UNARY_ITERATOR_TYPEDEFS(Base)\
	BOOST_RTL_PULL_ITERATOR_TYPEDEFS(Base)\
	typedef typename Base::arg_iterator arg_iterator;

#define BOOST_RTL_PULL_UNARY_OPERATOR_TYPEDEFS(Base)\
	typedef typename Base::field_list field_list;\
	typedef typename Base::sort_list sort_list;\
	typedef typename Base::value_type value_type;\
	typedef typename Base::const_iterator const_iterator;\
	typedef typename Base::arg_type arg_type;

namespace boost { namespace rel {

	// unary_iterator

	template <class Derived, class Relation> 
		class unary_iterator : public iterator_root<Derived, Relation>
	{
		typedef iterator_root<Derived, Relation> base_type;

	public:
		BOOST_RTL_PULL_ITERATOR_TYPEDEFS(base_type);
        using base_type::relation_;
		typedef typename Relation::arg_type::const_iterator arg_iterator;

		const arg_iterator& base() const 
		{
			return itr_;
		}
	public:
		bool operator== (const Derived& other) const 
		{
			return 
				itr_ == other.itr_ &&
				relation_ == other.relation_;
		}
	protected:
		unary_iterator(const arg_iterator& itr, const Relation* o) 
			: base_type(o)
			, itr_(itr)
		{}
		unary_iterator()
			: base_type(0)
			, itr_(arg_iterator())
		{}
	protected:
		arg_iterator itr_;
	};

	// unary_operator

	template<
		class Arg,
		class FieldList,
		class SortList,
		class Iterator
		>
	class unary_operator
	{
	public:
		typedef FieldList field_list;
		typedef SortList sort_list;
		typedef row<field_list> value_type;
		typedef Iterator const_iterator;
		typedef Arg arg_type;

	protected:
		unary_operator(const Arg& arg) 
			: arg_(arg) 
		{} 
		Arg arg_;

	public:
		Arg argument() const
		{
			return arg_;
		}
		template<class F>
			void traverse_args(F f) const
		{
			traverse(argument(), f);
		}
	};
}}

#endif//BOOST_RTL_UNARY_HPP_INCLUDED
