// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_KEY_INDEX_HPP_INCLUDED
#define BOOST_RTL_KEY_INDEX_HPP_INCLUDED

#include <boost/rtl/index.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/list.hpp>

namespace boost { namespace rel {

	template<class Table,class SortList>
	struct sit_
	{
		typedef row<typename list_union<SortList,typename Table::sort_list>::type> result;
	};

	template<class Table, class SortList, class FastList, class Strategy> 
    class key_idx_t;

	template<class Table, class SortList, class FastList, class Strategy> 
    class key_index_iterator : public index_iterator<
		key_index_iterator<Table, SortList, FastList, Strategy>,
		key_idx_t<Table, SortList, FastList, Strategy> 
    > 
    {
		typedef index_iterator<
            key_index_iterator<Table, SortList, FastList, Strategy>,
            key_idx_t<Table, SortList, FastList, Strategy> 
		> base_type;

		friend class index<
			Table,
			SortList,
			typename sit_<Table, FastList>::result,
			Strategy,
			key_index_iterator<Table, SortList, FastList, Strategy>,
			key_idx_t<Table, SortList, FastList, Strategy> >;

	public:
		BOOST_RTL_PULL_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr_;
		typedef typename base_type::idx_iterator idx_iterator;
		typedef typename relation_type::primary_tuple primary_tuple;

	private:
		key_index_iterator(const idx_iterator& itr, const relation_type* o) 
			: base_type(itr, o)
		{
		};

	public:
		key_index_iterator() 
		{
		};

		const idx_iterator& getIndexIterator() const {return itr_;}
	private:
		
		struct getFast
		{
			getFast(const key_index_iterator& itr) : i_(itr) {}
			template<class Column>
			typename Column::type
			get(const Column& c) const
			{
				return i_.getIndexIterator().get(c);
			}
			const key_index_iterator& i_;	
		};

		struct getSlow
		{
			getSlow(const key_index_iterator& itr) : i_(itr) {}
			template<class Column>
				typename Column::type get(const Column& c) const
			{
				primary_tuple sub;
				copy_fields<typename primary_tuple::field_list>(i_.getIndexIterator(), sub);
				typename Table::const_iterator itr(i_.relation()->argument().lower_bound(sub));
				return itr.get(c);
			}
			const key_index_iterator& i_;	
		};

	public:
		template<class Column>
			typename Column::type
		get_internal(const Column& c) const 
		{
			typedef typename boost::mpl::if_c
			<
				list_contains<typename relation_type::index_fields, Column>::value,
				getFast,
				getSlow
			>::type result;
			return result(*this).get(c);
		}
	};

	template<class Table, class SortList, class FastList, class Strategy>
	struct key_idx_base : boost::mpl::identity<index<
		Table,
		SortList,
		typename sit_<Table, FastList>::result,
		Strategy,
		key_index_iterator<Table, SortList, FastList, Strategy>,
		key_idx_t<Table, SortList, FastList, Strategy>
	> > {};

    struct key_index_tag {};

	template<class Table, class SortList, class FastList, class Strategy> 
		class key_idx_t : public key_idx_base<Table, SortList, FastList, Strategy>::type
	{
		typedef typename key_idx_base<Table, SortList, FastList, Strategy>::type base_type;
		friend class key_index_iterator<Table, SortList, FastList, Strategy>;

	public:
        typedef key_index_tag tag;

		BOOST_RTL_PULL_INDEX_TYPEDEFS(base_type);
        using base_type::index_;
		typedef typename arg_type::const_iterator arg_iterator;
		typedef row<typename Table::sort_list> primary_tuple;
		typedef typename index_tuple::field_list index_fields;
        typedef FastList fast_list;

		key_idx_t(const Table& t = Table()) : base_type(t)
		{
			// m_index.reserve(...); // if we can calculate it easy
			arg_iterator e = t.end();
			for(arg_iterator itr = t.begin(); itr != e; ++itr)
			{
				index_tuple temp;
				copy_fields<index_fields>(itr, temp);
				index_.insert(temp);
			}
		}
	};

    // basic key index

    template<
        class Table, 
        class SortList, 
        class FastList,
        class Strategy = default_table_implementation
    > 
	struct basic_key_index_t 
	{
        typedef key_idx_t<
            Table, 
            SortList, 
            FastList, 
            Strategy
        > type;
	};

	template<
        class SortList, 
        class FastList,
        class Table // deduced
    > 
	typename basic_key_index_t<Table, SortList, FastList>::type 
	basic_key_index(const Table& t)
	{
		return typename basic_key_index_t<Table, SortList, FastList>::type(t);
	}

    template<
        class SortList, 
        class FastList,
        class Strategy, // deduced 
        class Table // deduced
    > 
	typename basic_key_index_t<Table, SortList, FastList, Strategy>::type 
	basic_key_index(const Table& t, Strategy strategy)
	{
		return typename basic_key_index_t<Table, SortList, FastList, Strategy>::type(t);
	}

    // key index

    template<
        class Table, 
        class SortList, 
        class Strategy = default_table_implementation
    > 
    struct key_index_t : basic_key_index_t<
        Table, 
        SortList, 
        SortList, // FastList = SortList 
        Strategy
    > {};

	template<
        class SortList, 
        class Table // deduced
    > 
	typename key_index_t<Table, SortList>::type 
	key_index(const Table& t)
	{
		return typename key_index_t<Table, SortList>::type(t);
	}

    template<
        class SortList, 
        class Strategy, // deduced 
        class Table // deduced
    > 
    typename key_index_t<Table, SortList, Strategy>::type 
	key_index(const Table& t)
	{
		return typename key_index_t<Table, SortList, Strategy>::type(t);
	}

    // materialized index 

    template<
        class Table, 
        class SortList, 
        class Strategy = default_table_implementation
    > 
    struct materialized_index_t : basic_key_index_t<
        Table, 
        SortList, 
        typename Table::field_list, 
        Strategy
    > {};

	template<
        class SortList, 
        class Table // deduced
    > 
	typename materialized_index_t<Table, SortList>::type 
	materialized_index(const Table& t)
	{
		return typename materialized_index_t<Table, SortList>::type(t);
	}

    template<
        class SortList, 
        class Strategy, // deduced 
        class Table // deduced
    > 
	typename materialized_index_t<Table, SortList, Strategy>::type 
	materialized_index(const Table& t, Strategy strategy)
	{
		return typename materialized_index_t<Table, SortList, Strategy>::type(t);
	}

}}

#endif//BOOST_RTL_KEY_INDEX_HPP_INCLUDED
