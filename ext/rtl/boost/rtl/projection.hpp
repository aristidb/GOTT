// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_PROJECTION_HPP_INCLUDED
#define BOOST_RTL_PROJECTION_HPP_INCLUDED

#include <boost/rtl/unary.hpp>
#include <boost/rtl/list.hpp>
#include <boost/rtl/keys.hpp>

namespace boost { namespace rel {

	template<class Table, class FieldList> class dprj_t;
	template<class Table, class FieldList> 
		class dprj_iterator : public unary_iterator
		<
			dprj_iterator<Table, FieldList>,
			dprj_t<Table,FieldList>
		>
	{
		friend class unary_iterator
		<
			dprj_iterator<Table, FieldList>,
			dprj_t<Table,FieldList>
		>;
		friend class iterator_root
		<
			dprj_iterator<Table, FieldList>,
			dprj_t<Table,FieldList>
		>;
		typedef unary_iterator
		<
			dprj_iterator<Table, FieldList>,
			dprj_t<Table,FieldList>
		> base_type;

	public:
		BOOST_RTL_PULL_UNARY_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr_;
        using base_type::relation_;

		typedef typename relation_type::sort_list compare_list;

		dprj_iterator()
		{}
		
		template<class Column>
			typename Column::type get_internal(const Column& c) const
		{
			return itr_.get(c);
		}

		dprj_iterator(const arg_iterator& itr, const relation_type* o)
			: base_type(itr,o)
		{}

	private:
		
		void increment()
		{
			const arg_iterator e(relation_->arg_.end());
			const arg_iterator temp(itr_);
			do
			{
				++itr_;
			} while ((itr_ != e) && (compare_iterators<compare_list>(itr_,temp) == 0));
		}

		void decrement()
		{
			const arg_iterator temp(--itr_);
			const arg_iterator b(relation_->arg_.begin());
			while (itr_ != b)
			{
				--itr_;
				if (compare_iterators<compare_list>(itr_,temp) != 0)
				{
					++itr_;
					break;
				}
			}
		}
	};

	template<class Seq>
	struct doesnt_contain
	{
		template<class T>
			struct apply : boost::is_same<
				typename boost::mpl::find<Seq, T>::type,
				typename boost::mpl::end<Seq>::type
			>{};
	};

	template<class SList, class FList>
	struct find_first_not_there : boost::mpl::find_if<
		SList,
		doesnt_contain<FList>
	>{};

	template<class SList, class FList>
	struct up_to_first_not_there : boost::mpl::iterator_range<
		typename boost::mpl::begin<SList>::type,
		typename find_first_not_there<SList, FList>::type
	>{};


	template<class Table, class FieldList> 
		struct dprj_base : boost::mpl::identity
		<
			unary_operator
			<
				Table,
				FieldList,
				typename up_to_first_not_there<typename Table::sort_list,FieldList>::type,
				dprj_iterator<Table, FieldList>
			>
		>
	{};

    struct projection_tag {};

	template<class Table, class FieldList>
		class dprj_t : public dprj_base<Table,FieldList>::type
	{
		typedef typename dprj_base<Table,FieldList>::type base_type;

		friend class dprj_iterator<Table,FieldList>;
	public:
        typedef projection_tag tag;

		BOOST_RTL_PULL_UNARY_OPERATOR_TYPEDEFS(base_type);
        using base_type::arg_;

		typedef FieldList restriction;

		typedef typename key_add
		<
			sort_list,
			typename projection_keys<typename Table::keys, FieldList>::type
		>::type keys;

		dprj_t(const Table& t)
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

			range_t<typename arg_type::const_iterator> r(arg_.equal_range(sub));
			dprj_t<range_t<typename arg_type::const_iterator>, FieldList> p(r);
			
			const_iterator b(p.begin().base().base(),this);
			const_iterator e(p.end().base().base(),this);
			return range_t<const_iterator>(b,e);
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			range_t<typename arg_type::const_iterator> r(arg_.lower_bound(sub),arg_.end());
			dprj_t<range_t<typename arg_type::const_iterator>,FieldList> s(r);
			return const_iterator(s.begin().base().base(),this);
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);
			range_t<typename arg_type::const_iterator> r(arg_.begin(),arg_.upper_bound(sub));
			dprj_t<range_t<typename arg_type::const_iterator>,FieldList> s(r);
			return const_iterator(s.end().base().base(),this);
		}
	};

	template<class Table, class FieldList>
		struct projection_t
	{
		typedef dprj_t<Table,FieldList> type;
	};


#if defined(BOOST_MPL_MSVC_ETI_BUG)
	template<>
		struct projection_t<int,int>
	{
		typedef int type;
	};
#endif


	template<class FieldList, class Table>
		typename projection_t<Table,FieldList>::type projection(const Table& t, const FieldList* = 0)
	{
		return typename projection_t<Table,FieldList>::type(t);
	}
		
}}

#endif

