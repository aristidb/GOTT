// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov 
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_EXTENSION_HPP_INCLUDED
#define BOOST_RTL_EXTENSION_HPP_INCLUDED

#include <boost/rtl/unary.hpp>

namespace boost { namespace rel {

	template<class Table, class Policy> class ext_t;
	template<class Table, class Policy> class extension_iterator;

	template<class Table, class Policy> 
		class extension_iterator : public unary_iterator<
			extension_iterator<Table, Policy>, ext_t<Table, Policy>
		>
	{
		typedef unary_iterator<
			extension_iterator<Table, Policy>, ext_t<Table, Policy>
		> base_type;

		friend class iterator_root<extension_iterator<Table, Policy>, ext_t<Table, Policy> >;

	public:
		BOOST_RTL_PULL_UNARY_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr_;

		extension_iterator(const arg_iterator& itr, const relation_type* o) 
			: base_type(itr, o) 
		{}
		
		extension_iterator() 
		{}

	private:
		template<class Column> 
			typename Column::type get_internal(const Column& c) const 
		{
			typedef typename list_contains<typename Table::field_list, Column>::type in_arg;
			return get_internal(c, (in_arg*)0);
		}
		template<class Column> 
			typename Column::type get_internal(const Column& c, const boost::mpl::true_*) const 
		{
			return itr_.get(c);
		}
		template<class Column> 
			typename Column::type get_internal(const Column& c, const boost::mpl::false_*) const 
		{
			return this->relation()->policy().get(itr_, c);
		}
			
		void increment(){++itr_;}
		void decrement(){--itr_;}
	};

	// ext_base

	template<class Table, class Policy>
		struct ext_base : boost::mpl::identity<unary_operator<
			Table,
			typename concat<typename Table::field_list, typename Policy::field_list>::type,
			typename Table::sort_list,
			extension_iterator<Table, Policy>
		> > {};

	// ext_t

    struct extension_tag {};

	template<class Table, class Policy>
		class ext_t : public ext_base<Table, Policy>::type
	{
		typedef typename ext_base<Table, Policy>::type base_type;
		friend class extension_iterator<Table, Policy>;

	public:
        typedef extension_tag tag;

		BOOST_RTL_PULL_UNARY_OPERATOR_TYPEDEFS(base_type);
        using base_type::arg_;
		typedef typename Table::keys keys;

		typedef Policy policy_type;

	public:
		ext_t(const Table& t, const Policy& p) 
			: base_type(t)
			, p_(p)
		{}
		const_iterator begin() const 
		{
			return const_iterator(arg_.begin(), this);
		}
		const_iterator end() const 
		{
			return const_iterator(arg_.end(), this);
		}
		template <class SubTuple>
			range_t<const_iterator> equal_range(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple, sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			range_t<typename arg_type::const_iterator> r(arg_.equal_range(sub));
			return range(
				const_iterator(r.begin().base(), this),
				const_iterator(r.end().base(), this));
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple, sort_list>::value;
			BOOST_STATIC_ASSERT(valid);
			return const_iterator(arg_.lower_bound(sub), this);
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple, sort_list>::value;
			BOOST_STATIC_ASSERT(valid);
			return const_iterator(arg_.upper_bound(sub), this);
		}
		Policy policy() const
		{
			return p_;
		}
	private:
		Policy p_;
	};

	// extension_t

	template<class Table, class Policy>
		struct extension_t : boost::mpl::identity<ext_t<Table, Policy> >
	{};

	// extension

	template<class Table, class Policy>
		typename extension_t<Table, Policy>::type extension(const Table& t, const Policy& p)
	{
		return typename extension_t<Table, Policy>::type(t, p);
	}
}}

#endif//BOOST_RTL_EXTENSION_HPP_INCLUDED
