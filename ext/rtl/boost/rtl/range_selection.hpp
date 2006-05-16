// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_RANGE_SELECTION_HPP_INCLUDED
#define BOOST_RTL_RANGE_SELECTION_HPP_INCLUDED

#include <boost/rtl/range.hpp>
#include <boost/rtl/range_functors.hpp>

namespace boost { namespace rel {

	// range_selection

	template<class Table, class Begin, class End> 
		class rsel_t;

	template<class Table, class Begin, class End>
		class range_selection_policy
	{
		friend class rsel_t<Table, Begin, End>;

	public:
		typedef typename Table::const_iterator iterator;
		typedef rsel_t<Table, Begin, End> relation;
		
		range_selection_policy(const Table& t, const Begin& b, const End& e)
			: t_(t)
			, begin_(b)
			, end_(e)
		{}

		iterator begin() const {return begin_(t_);}
		iterator end() const {return end_(t_);}

	private:
		Table t_;
		Begin begin_;
		End end_;
	};

    struct range_selection_tag {};

	template<class Table, class Begin, class End> 
		class rsel_t : public basic_range<range_selection_policy<Table, Begin, End> >
	{
        typedef basic_range<range_selection_policy<Table, Begin, End> > base_type;
        using base_type::policy_;

	public:
        typedef range_selection_tag tag;

		typedef Table arg_type;
		typedef Begin begin_type;
		typedef End end_type;

		rsel_t(const Table& t, const Begin& b, const End& e)
			: basic_range<range_selection_policy<Table, Begin, End> >(range_selection_policy<Table, Begin, End>(t, b, e))
		{}
		arg_type argument() const
		{
			return policy_.t_;
		}
		Begin begin_functor() const
		{
			return policy_.begin_;
		}
		End end_functor() const
		{
			return policy_.end_;
		}
		template<class F> 
			void traverse_args(F f) const
		{
			traverse(policy_.t_, f);
		}
	};

	template<class Table, class Begin, class End>
		struct range_selection_t : boost::mpl::identity<rsel_t<Table, Begin, End> >
	{};

	template<class Table, class Begin, class End> 
		typename range_selection_t<Table, Begin, End>::type range_selection(const Table& t, const Begin& b, const End& e)
	{
		return typename range_selection_t<Table, Begin, End>::type(t, b, e);
	}

	// equal_selection

	template<class Table, class Sub> 
		class esel_t;

	template<class Table, class Sub>
		class equal_selection_policy
	{
		friend class esel_t<Table, Sub>;

	public:
		typedef typename Table::const_iterator iterator;
		typedef esel_t<Table, Sub> relation;
		
		equal_selection_policy(const Table& t, const Sub& sub)
			: t_(t)
			, sub_(sub)
		{}

		// re-implement to use equal_range.  need to re-init after transaction committed;
		iterator begin() const {return t_.lower_bound(sub_);}
		iterator end() const {return t_.upper_bound(sub_);}

	private:
		Table t_;
		Sub sub_;
	};

    struct equal_selection_tag {};

	template<class Table, class Sub> 
		class esel_t : public basic_range<equal_selection_policy<Table, Sub> >
	{
        typedef basic_range<equal_selection_policy<Table, Sub> > base_type;
        using base_type::policy_;

	public:
        typedef equal_selection_tag tag;

		typedef Table arg_type;
		typedef Sub sub_type;

		esel_t(const Table& t, const Sub& sub)
			: basic_range<equal_selection_policy<Table, Sub> >(equal_selection_policy<Table, Sub>(t, sub))
		{}
		arg_type argument() const
		{
			return policy_.t_;
		}
		Sub sub() const
		{
			return policy_.sub_;
		}
		template<class F> 
			void traverse_args(F f) const
		{
			traverse(policy_.t_, f);
		}
	};

	template<class Table, class Sub> 
		struct equal_selection_t : boost::mpl::identity<esel_t<Table, Sub> >
	{};

	template<class Table, class Sub> 
		typename equal_selection_t<Table, Sub>::type equal_selection(const Table& t, const Sub& sub)
	{
		return typename equal_selection_t<Table, Sub>::type(t, sub);
	}

	// eq
	template<class Table, class Key> 
		struct selection_eq_t : equal_selection_t<Table, Key>
	{};
	template<class Table, class Key>
		typename selection_eq_t<Table, Key>::type
		selection_eq(const Table& t, const Key& k)
	{
		return equal_selection(t, k);
	}

	// lt
	template<class Table, class Key> 
		struct selection_lt_t : boost::mpl::identity<
		typename range_selection_t<Table, begin, lower_bound_t<Key> >::type>
	{};
	template<class Table, class Key>
		typename selection_lt_t<Table, Key>::type
		selection_lt(const Table& t, const Key& k)
	{
		return range_selection(t, begin(), lower_bound(k));
	}

	// le
	template<class Table, class Key> 
		struct selection_le_t : boost::mpl::identity<
		typename range_selection_t<Table, begin, upper_bound_t<Key> >::type>
	{};
	template<class Table, class Key>
		typename selection_le_t<Table, Key>::type
		selection_le(const Table& t, const Key& k)
	{
		return range_selection(t, begin(), upper_bound(k));
	}

	// gt
	template<class Table, class Key> 
		struct selection_gt_t : boost::mpl::identity<
		typename range_selection_t<Table, upper_bound_t<Key>, end>::type>
	{};
	template<class Table, class Key>
		typename selection_gt_t<Table, Key>::type
		selection_gt(const Table& t, const Key& k)
	{
		return range_selection(t, upper_bound(k), end());
	}

	//ge
	template<class Table, class Key> 
		struct selection_ge_t : boost::mpl::identity<
		typename range_selection_t<Table, lower_bound_t<Key>, end>::type>
	{};
	template<class Table, class Key>
		typename selection_ge_t<Table, Key>::type
		selection_ge(const Table& t, const Key& k)
	{
		return range_selection(t, lower_bound(k), end());
	}
}}

#endif//BOOST_RTL_RANGE_SELECTION_HPP_INCLUDED
