// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_GROUPBY_HPP_INCLUDED
#define BOOST_RTL_GROUPBY_HPP_INCLUDED

#include <boost/rtl/unary.hpp>
#include <boost/rtl/list.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/rtl/keys.hpp>

namespace boost { namespace rel {

	// group_field

	template <class It, class TargetTuple> 
		class group_field
	{
	public:
		group_field(const It& it, TargetTuple& trg)
			: it_(it)
			, trg_(trg) 
		{}

		template <class T> 
			void operator()(const T&)
		{
			static_cast<typename T::impl_type&>(trg_.fields_)(it_);
		}
	private:
		const It& it_;
		TargetTuple& trg_;
	};

	template <class FieldList, class It, class TargetTuple>
	void group_fields(const It& it, TargetTuple& trg)
	{
		boost::mpl::for_each<FieldList>(group_field<It, TargetTuple>(it, trg));
	}

	// groupby_iterator

	template <class Table, class GroupList, int PrefixSize> class gby_t;

	template <class Table, class GroupList, int PrefixSize> 
		class groupby_iterator : public unary_iterator
		<
			groupby_iterator<Table, GroupList, PrefixSize>, 
			gby_t<Table, GroupList, PrefixSize> 
		>
	{
		typedef unary_iterator
		<
			groupby_iterator<Table, GroupList, PrefixSize>, 
			gby_t<Table, GroupList, PrefixSize> 
		> base_type;

		friend class iterator_root<groupby_iterator<Table, GroupList, PrefixSize>, gby_t<Table, GroupList, PrefixSize> >;
		friend class gby_t<Table,GroupList,PrefixSize>;

	public:
		BOOST_RTL_PULL_UNARY_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr_;
        using base_type::relation_;

		typedef typename list_prefix<typename Table::sort_list,PrefixSize>::type key_list;
		BOOST_STATIC_CONSTANT(int, prefix_size=PrefixSize);

		groupby_iterator(const arg_iterator& itr, const relation_type* o) 
			: base_type(itr, o) 
		{}

		groupby_iterator() 
		{} 

        template<class Column>
            typename Column::type _get(const Column& c, boost::mpl::true_) const
        {
            return itr_[c];
        }

        template<class Column>
            typename Column::type _get(const Column& c, boost::mpl::false_) const
        {
            typename Column::impl_type result;

            row<key_list> sub;
            copy_fields<key_list>(itr_, sub);

            arg_iterator end = relation_->arg_.upper_bound(sub);

            for(arg_iterator it = itr_; it != end; ++it)
				result(it);

            return result.value();
        }

		template<class Column>
			typename Column::type get_internal(const Column& c) const 
		{
            typedef typename list_contains<
                typename relation_type::key_list, 
                Column
            >::type is_fast; 
            return _get(c, is_fast()); 
		}

	private:
		void increment()
		{
            row<key_list> sub;
            copy_fields<key_list>(itr_, sub);
            *this = groupby_iterator(relation_->arg_.upper_bound(sub), relation_);
		}
		
		void decrement()
		{
            --itr_;
            row<key_list> sub;
            copy_fields<key_list>(itr_, sub);
            *this = groupby_iterator(relation_->arg_.lower_bound(sub), relation_);
		}

	private:
		value_type result_;
	};

	// gby_base

	template <class Table, class GroupList, int PrefixSize>
        struct gby_base : boost::mpl::identity<unary_operator<
			Table,
			typename concat
			<
				typename list_prefix<typename Table::sort_list,PrefixSize>::type, 
				GroupList
			>::type,
			typename list_prefix<typename Table::sort_list,PrefixSize>::type,
			groupby_iterator<Table, GroupList, PrefixSize>
		> > {};

	// gby_t

    struct groupby_tag {};

	template <class Table, class GroupList, int PrefixSize>
		class gby_t : public gby_base<Table, GroupList, PrefixSize>::type
	{
		typedef typename gby_base<Table, GroupList, PrefixSize>::type base_type;
		friend class groupby_iterator<Table, GroupList, PrefixSize>;

	public:
        typedef groupby_tag tag;

		BOOST_RTL_PULL_UNARY_OPERATOR_TYPEDEFS(base_type);
        using base_type::arg_;
		typedef typename list_prefix<typename Table::sort_list, PrefixSize>::type key_list;
		typedef GroupList group_list;
        enum{prefix_size = PrefixSize};
		typedef boost::mpl::vector1<sort_list> keys; // all keys are discarded, except new sort_list

		gby_t(const arg_type& x) 
			: base_type(x) 
		{}

		const_iterator begin() const 
		{
            return const_iterator(arg_.begin(), this);
		}

		const_iterator end() const
		{
			return const_iterator(arg_.end(), this);
		}

		template<class SubTuple>
			range_t<const_iterator> equal_range(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			range_t<typename Table::const_iterator> r(arg_.equal_range(sub));

			const_iterator lbound(r.begin().base(), this);
			const_iterator ubound(r.end().base(), this);

			return range(lbound, ubound);
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			return const_iterator(arg_.lower_bound(sub), this);
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			return const_iterator(arg_.upper_bound(sub), this);
		}
	};

	// groupby
	template <class Table, class GroupList, int PrefixSize>
		struct groupby_t
	{
		typedef gby_t<Table, GroupList, PrefixSize> type;
	};

	template <int PrefixSize, class GroupList, class Table>
		typename groupby_t<Table, GroupList, PrefixSize>::type 
	groupby(const Table& t)
	{
		return typename groupby_t<Table, GroupList, PrefixSize>::type(t);
	}

}}

#endif//BOOST_RTL_GROUPBY_HPP_INCLUDED
