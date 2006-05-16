// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_CACHE_HPP_INCLUDED
#define BOOST_RTL_CACHE_HPP_INCLUDED

#include <boost/rtl/utils.hpp>
#include <boost/rtl/unary.hpp>
#include <boost/rtl/range.hpp>
#include <boost/rtl/utils.hpp>
#include <boost/rtl/identifiable.hpp>

namespace boost { namespace rel {

	template<class Table> class ch_t;

	template<class Table> 
		class cache_iterator : public unary_iterator
		<
			cache_iterator<Table>, 
			ch_t<Table> 
		>
	{
		typedef unary_iterator
		<
			cache_iterator<Table>, 
			ch_t<Table> 
		> base_type;

		friend class iterator_root<cache_iterator<Table>, ch_t<Table> >;
		friend class unary_iterator<cache_iterator<Table>, ch_t<Table> >;
	
	public:
		BOOST_RTL_PULL_UNARY_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr_;

		cache_iterator()
		{}

		template<class Column>
			typename Column::type get_internal(const Column& c) const
		{
			return itr_.get(c);
		}

		cache_iterator(const arg_iterator& itr, const relation_type* o) 
			: base_type(itr,o)
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

	template<class Table>
		class cache_state
	{
	public:
		cache_state()
			: has_begin_(false)
			, has_end_(false)
		{}
		void clear()
		{
			has_begin_ = false;
			has_end_ = false;
		}
		typename Table::const_iterator begin(const Table& t)
		{
			if (!has_begin_)
			{
				begin_ = t.begin();
				has_begin_ = true;
			}
			return begin_;
		}
		typename Table::const_iterator end(const Table& t)
		{
			if (!has_end_)
			{
				end_ = t.end();
				has_end_ = true;
			}
			return end_;
		}

	private:
		bool has_begin_;
		bool has_end_;
		typename Table::const_iterator begin_;
		typename Table::const_iterator end_;
	};

	// ch_t

	/*
	All copies of cache share their argument, 
	so that each copy's iterators are valid 
	*/

    struct cache_tag {}; 

	template<class Table>
		class ch_t : public identifiable
	{
	public:
        typedef cache_tag tag;

		typedef typename Table::field_list field_list;
		typedef typename Table::sort_list sort_list;
		typedef typename Table::keys keys;
		typedef row<field_list> value_type;
		typedef cache_iterator<Table> const_iterator;
		typedef Table arg_type;

		typedef typename arg_type::const_iterator arg_iterator;

		ch_t(const Table& t) 
			: arg_ptr_(new Table(t))
			, state_(new cache_state<Table>())
		{}

		Table argument() const
		{
			return *arg_ptr_;
		}
		
		template<class F>
			void traverse_args(F f) const
		{
			traverse(*arg_ptr_, f);
		}

		void refill()
		{
			state_->clear();
		}

		const_iterator begin() const
		{
			return const_iterator(state_->begin(*arg_ptr_) ,this);
		}
		
		const_iterator end() const
		{
			return const_iterator(state_->end(*arg_ptr_), this);
		}

		template<class SubTuple>
			range_t<const_iterator> equal_range(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);
			range_t<typename Table::const_iterator> r(arg_ptr_->equal_range(sub));
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

			return const_iterator(arg_ptr_->lower_bound(sub),this);
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			return const_iterator(arg_ptr_->upper_bound(sub),this);
		}
	
	private:
		boost::shared_ptr<Table> arg_ptr_;
		mutable boost::shared_ptr<cache_state<Table> > state_; 
	};

	template<class Table>
		struct cache_t
	{
		typedef ch_t<Table> type;
	};

	template<class Table>
	typename cache_t<Table>::type
	cache(const Table& t)
	{
		return typename cache_t<Table>::type(t);
	}
}}

#endif//BOOST_RTL_CACHE_HPP_INCLUDED

