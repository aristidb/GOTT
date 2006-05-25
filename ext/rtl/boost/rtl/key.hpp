// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_KEY_HPP_INCLUDED
#define BOOST_RTL_KEY_HPP_INCLUDED

#include <boost/rtl/list.hpp>
#include <boost/rtl/keys.hpp>
#include <boost/rtl/unary.hpp>

namespace boost { namespace rel {

	template<class Table, class Keys> class k_t;
	template<class Table, class Keys>
		class key_iterator : public unary_iterator
		<
			key_iterator<Table,Keys>,
			k_t<Table,Keys>
		>
	{
		typedef unary_iterator
		<
			key_iterator<Table,Keys>, 
			k_t<Table,Keys> 
		> base_type;

		friend class iterator_root<key_iterator<Table,Keys>, k_t<Table,Keys> >;
		friend class unary_iterator<key_iterator<Table,Keys>, k_t<Table,Keys> >;
	public:
		BOOST_RTL_PULL_UNARY_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr_;

		key_iterator() 
		{}
	
		key_iterator(const arg_iterator& itr, const relation_type* o)
			: base_type(itr,o)
		{}

	private:
		template<class Column>
			typename Column::type
		get_internal(const Column& c) const
		{
			return itr_.get(c);
		}

		void increment()
		{
			++itr_;
		}

		void decrement()
		{
			--itr_;
		}
	};

	template<class Table, class Keys>
		struct apply_key
	{
		typedef typename Table::keys old_keys;
		typedef Keys new_keys;
		typedef typename Table::sort_list old_sort_list;
		typedef typename keys_concat<old_keys,new_keys>::type keys;
		typedef typename optimize_sort<typename Table::sort_list,keys>::type sort_list;
	};

	template <class Table, class Keys>
		struct key_base : boost::mpl::identity
		<
			unary_operator
			<
				Table,
				typename Table::field_list,
				typename apply_key<Table,Keys>::sort_list,
				key_iterator<Table,Keys>
			> 
		> 
	{};

    struct key_tag {};

	template<class Table, class Keys>
		class k_t : public key_base<Table,Keys>::type
	{
		typedef typename key_base<Table, Keys>::type base_type;
		friend class key_iterator<Table, Keys>;
	public:
        typedef key_tag tag;

		BOOST_RTL_PULL_UNARY_OPERATOR_TYPEDEFS(base_type);
        using base_type::arg_;

		typedef Keys key_type; // just parameter, not
		typedef typename apply_key<Table,Keys>::keys keys;

		k_t(const Table& t) 
			: base_type(t)
		{}

		const_iterator begin() const
		{
			return const_iterator(arg_.begin(),this);
		}

		const_iterator end() const
		{
			return const_iterator(arg_.end(),this);
		}

		template<class SubTuple>
			range_t<const_iterator> equal_range(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);
			range_t<typename Table::const_iterator> r(arg_.equal_range(sub));
			return range_t<const_iterator>
			(
				const_iterator(r.begin().base(),this), 
				const_iterator(r.end().base(),this)
			);
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			return const_iterator(arg_.lower_bound(sub),this);
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			return const_iterator(arg_.upper_bound(sub),this);
		}
	};

	template<class Table, class Keys>
		struct key_t
	{
		typedef k_t<Table,Keys> type;
	};

	template<class Keys, class Table>
		typename key_t<Table, Keys>::type key(const Table& t, const Keys* = 0)
	{
		return typename key_t<Table,Keys>::type(t);
	}

}}

#endif

