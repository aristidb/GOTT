// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_RENAME_HPP_INCLUDED
#define BOOST_RTL_RENAME_HPP_INCLUDED

#include <boost/rtl/list.hpp>
#include <boost/rtl/keys.hpp>
#include <boost/rtl/unary.hpp>
#include <boost/rtl/range.hpp>

namespace boost { namespace rel {

	template<class Table, class Pairs> class cnv_t;

	template<class Table, class Pairs> class conversion_iterator 
		: public unary_iterator
		<
			conversion_iterator<Table, Pairs>, 
			cnv_t<Table,Pairs>
		>
	{
		typedef unary_iterator
		<
			conversion_iterator<Table, Pairs>, 
			cnv_t<Table,Pairs>
		> base_type;

		friend class iterator_root
		<
			conversion_iterator<Table, Pairs>, 
			cnv_t<Table,Pairs>
		>;

	public:
		BOOST_RTL_PULL_UNARY_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr_;

		conversion_iterator() 
		{}

		template<class Column>
			typename Column::type
		get_internal(const Column&) const 
		{
			/*
			Some weird MSVC6 bug requires the code below.
			Otherwise INTERNAL COMPILER ERROR occurs.  AV
			*/
			#if defined(BOOST_MSVC) && BOOST_MSVC < 1300
				typedef arg_iterator::value_type::field_list _;
			#endif
			/**/

			typedef typename find_value
			<
				typename relation_type::reverse_pairs, 
				Column
			>::type original_column;
			
			return itr_.get(original_column());
		}

	
		conversion_iterator(const arg_iterator& itr, const relation_type* o) 
			: base_type(itr, o) 
		{}

	private:

		void increment()
		{
			++itr_;
		}
		void decrement()
		{
			--itr_;
		}
	};

	// conversion_base

	template<class Table, class Pairs>
		struct cnv_base : boost::mpl::identity
		<
			unary_operator
			<
				Table, 
				typename full_sublist_conversion<Pairs, typename Table::field_list>::type,
				typename short_sublist_conversion<Pairs, typename Table::sort_list>::type,
				conversion_iterator<Table, Pairs>
			> 
		> 
	{};

	// cnv_t

    struct conversion_tag {};

	template<class Table, class Pairs>
		class cnv_t : public cnv_base<Table, Pairs>::type
	{
		typedef typename cnv_base<Table, Pairs>::type base_type;

		typedef typename make_reverse<Pairs>::type reverse_pairs;

		friend class conversion_iterator<Table,Pairs>;
	
    public:
        typedef conversion_tag tag;

		BOOST_RTL_PULL_UNARY_OPERATOR_TYPEDEFS(base_type);
        using base_type::arg_;

		typedef Pairs pairs;

		typedef typename rename_keys
		<
			typename Table::keys, 
			Pairs
		>::type keys;

		cnv_t(const Table& t) 
			: base_type(t) 
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

			typedef row
			<
				typename full_sublist_conversion
				<
					reverse_pairs,
					typename SubTuple::field_list
				>::type
			> target_subtuple;
			target_subtuple tup;
			
			boost::mpl::for_each
			<
				typename filter_pairs<reverse_pairs,typename SubTuple::field_list>::type
			>(copy_different_fields<SubTuple,target_subtuple>(sub,tup));
			
			range_t<typename arg_type::const_iterator> r(arg_.equal_range(tup));
			const_iterator cb(r.begin().base(),this);
			const_iterator ce(r.end().base(),this);
			return range_t<const_iterator>(cb,ce);
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			typedef row
			<
				typename full_sublist_conversion
				<
					reverse_pairs,
					typename SubTuple::field_list
				>::type
			> target_subtuple;
			target_subtuple tup;
			boost::mpl::for_each
			<
				typename filter_pairs<reverse_pairs,typename SubTuple::field_list>::type
			>(copy_different_fields<SubTuple,target_subtuple>(sub,tup));
			return const_iterator(arg_.lower_bound(tup),this);
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			typedef row
			<
				typename full_sublist_conversion
				<
					reverse_pairs,
					typename SubTuple::field_list
				>::type
			> target_subtuple;
			target_subtuple tup;
			boost::mpl::for_each
			<
				typename filter_pairs<reverse_pairs,typename SubTuple::field_list>::type
			>(copy_different_fields<SubTuple,target_subtuple>(sub,tup));
			return const_iterator(arg_.upper_bound(tup),this);
		}
	};

	template<class Table, class FieldList>
		struct manual_rename_t
	{
		typedef cnv_t
		<
			Table,
			typename create_pairs<typename Table::field_list,FieldList,rel::size<FieldList>::value>::type
		> type;
	};
	
	template<class Table, class A = null_type>
		struct auto_rename_t
	{
		typedef cnv_t
		<
			Table,
			typename rename_pairs<typename Table::field_list,A>::type
		> type;
	};

	template<class FieldList, class Table>
		typename manual_rename_t<Table,FieldList>::type manual_rename(const Table& t)
	{
		return typename manual_rename_t<Table,FieldList>::type(t);
	}

	template<class A, class Table>
		typename auto_rename_t<Table,A>::type
		auto_rename(const Table& t)
	{
		return typename auto_rename_t<Table,A>::type(t);
	}

}}

#endif
