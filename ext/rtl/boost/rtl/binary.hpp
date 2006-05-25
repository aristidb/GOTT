// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_BINARY_HPP_INCLUDED
#define BOOST_RTL_BINARY_HPP_INCLUDED

#include <boost/rtl/tuple.hpp>
#include <boost/rtl/iterator_root.hpp>

#define BOOST_RTL_PULL_BINARY_ITERATOR_TYPEDEFS(Base)\
	BOOST_RTL_PULL_ITERATOR_TYPEDEFS(Base)\
	typedef typename Base::arg1_iterator arg1_iterator;\
	typedef typename Base::arg2_iterator arg2_iterator;

#define BOOST_RTL_PULL_BINARY_OPERATOR_TYPEDEFS(Base)\
	typedef typename Base::field_list field_list;\
	typedef typename Base::sort_list sort_list;\
	typedef typename Base::value_type value_type;\
	typedef typename Base::const_iterator const_iterator;\
	typedef typename Base::arg1_type arg1_type;\
	typedef typename Base::arg2_type arg2_type;

namespace boost { namespace rel {

    // binary_iterator
	
	template <class Derived, class Relation> 
		class binary_iterator : public iterator_root<Derived, Relation>
	{
		typedef iterator_root<Derived, Relation> base_type;

	public:
		BOOST_RTL_PULL_ITERATOR_TYPEDEFS(base_type);
		typedef typename Relation::arg1_type::const_iterator arg1_iterator;
		typedef typename Relation::arg2_type::const_iterator arg2_iterator;

		const arg1_iterator& base1() const 
		{
			return itr1_;
		}

		const arg2_iterator& base2() const 
		{
			return itr2_;
		}
	public:
		bool operator== (const Derived& other) const 
		{
			return 
				itr1_ == other.itr1_ &&
				itr2_ == other.itr2_ &&
				this->relation_ == other.relation_;
		}
	protected:
		binary_iterator(const arg1_iterator& itr1, const arg2_iterator& itr2, const Relation* o)
			: base_type(o)
			, itr1_(itr1)
			, itr2_(itr2)
		{}
		binary_iterator()
			: base_type(0) 
			, itr1_(arg1_iterator())
			, itr2_(arg2_iterator())
		{}
	protected:
		arg1_iterator itr1_;
		arg2_iterator itr2_;
	};

	// binary_operator

	template<
		class Arg1,
		class Arg2,
		class FieldList,
		class SortList,
		class Iterator
		>
	class binary_operator
	{
	public:
		typedef FieldList field_list;
		typedef SortList sort_list;
		typedef row<field_list> value_type;
		typedef Iterator const_iterator;
		typedef Arg1 arg1_type;
		typedef Arg2 arg2_type;

	protected:
		binary_operator(const Arg1& arg1, const Arg2& arg2) 
			: arg1_(arg1)
			, arg2_(arg2) 
		{}
		Arg1 arg1_;
		Arg2 arg2_;

	public:
		Arg1 argument1() const
		{
			return arg1_;
		}
		Arg2 argument2() const
		{
			return arg2_;
		}
		template<class F>
			void traverse_args(F f) const
		{
			traverse(argument1(), f);
			traverse(argument2(), f);
		}
	};
}}

#endif//BOOST_RTL_BINARY_HPP_INCLUDED
