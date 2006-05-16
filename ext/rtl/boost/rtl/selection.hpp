// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_SELECTION_HPP_INCLUDED
#define BOOST_RTL_SELECTION_HPP_INCLUDED

#include <boost/rtl/unary.hpp>
#include <boost/rtl/range.hpp>
#include <boost/rtl/lambda_support.hpp>

namespace boost { namespace rel {

	template <class Table, class Pred> class sel_t;
	template <class Table, class Pred> class selection_iterator;

	// selection_iterator

	template <class Table, class Pred> 
		class selection_iterator : public unary_iterator
		<
			selection_iterator<Table, Pred>, 
			sel_t<Table, Pred> 
		> 
	{
		typedef unary_iterator
		<
			selection_iterator<Table, Pred>, 
			sel_t<Table, Pred> 
		> base_type; 
		
		friend class iterator_root<selection_iterator<Table, Pred>, sel_t<Table, Pred> >;
		friend class sel_t<Table, Pred>;

	public:
		BOOST_RTL_PULL_UNARY_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr_;
        using base_type::relation_;

	private:
		selection_iterator(const arg_iterator& itr, const relation_type* o) 
			: base_type(itr, o) 
		{}
	public:
		selection_iterator() 
		{}
		template<class Column>
			typename Column::type get_internal(const Column& c) const 
		{
			return itr_.get(c);
		}

	private:
		void increment() 
		{
			++itr_;
			move_to_next();	
		}
		void decrement() 
		{
			--itr_;
			move_to_prev();	
		}
		void move_to_next()
		{
            arg_iterator end = relation_->arg_.end();

			while (itr_ != end && !(relation_->pred_)(itr_))
				++itr_;
		}
		void move_to_prev()
		{
			while (itr_ != relation_->arg_.begin())
			{
				//value_type tup(*itr_);
				//if ((relation_->pred_)(tup))

                if ((relation_->pred_)(itr_))
					break;

				--itr_; 
			}
		}
	};

	// sel_base

	template <class Table, class Pred>
		struct sel_base : boost::mpl::identity
		<
			unary_operator
			<
				Table,
				typename Table::field_list,
				typename Table::sort_list,
				selection_iterator<Table, Pred>
			> 
		> 
	{};

	// sel_t

    struct selection_tag {};

	template <class Table, class Pred>
		class sel_t : public sel_base<Table, Pred>::type
	{
		typedef typename sel_base<Table, Pred>::type base_type;
		friend class selection_iterator<Table, Pred>;

	public:
        typedef selection_tag tag;

		BOOST_RTL_PULL_UNARY_OPERATOR_TYPEDEFS(base_type);
        using base_type::arg_;
		typedef Pred pred_type;
		typedef typename Table::keys keys;

	public:
		sel_t(const Table& t, Pred pred) 
			: base_type(t)
			, pred_(pred) 
		{}
		//template<class CompatiblePred>
		//	sel_t(const sel_t<Table, CompatiblePred>& rhs)
		//	: base_type(rhs.argument())
		//	, pred_(rhs.pred()) 
		//{}
		const_iterator begin() const 
		{
			const_iterator i(arg_.begin(), this);
			i.move_to_next();
			return i;
		}
		const_iterator end() const 
		{
			return const_iterator(arg_.end(), this);
		}
		template <class SubTuple>
			range_t<const_iterator> equal_range(const SubTuple& sub) const
		{
			BOOST_STATIC_ASSERT((
                valid_subtuple<SubTuple,typename Table::sort_list>::value
                ));

			range_t<typename arg_type::const_iterator> r(arg_.equal_range(sub));

            typename arg_type::const_iterator it1 = r.begin().base();
            typename arg_type::const_iterator it2 = r.end().base();
            typename arg_type::const_iterator end = arg_.end();

            while(it1 != end && !pred_(it1))
                ++it1;

            while(it2 != end && !pred_(it2))
                ++it2;

            return range_t<const_iterator>(
                const_iterator(it1, this),
                const_iterator(it2, this));
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& sub) const
		{
			BOOST_STATIC_ASSERT((
                valid_subtuple<SubTuple,typename Table::sort_list>::value
                ));

            typename arg_type::const_iterator it = arg_.lower_bound(sub);
            typename arg_type::const_iterator end = arg_.end();

            while(it != end && !pred_(it))
                ++it;

            return const_iterator(it, this);
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& sub) const
		{
			BOOST_STATIC_ASSERT((
                valid_subtuple<SubTuple,typename Table::sort_list>::value
                ));

            typename arg_type::const_iterator it = arg_.upper_bound(sub);
            typename arg_type::const_iterator end = arg_.end();

            while(it != end && !pred_(it))
                ++it;

            return const_iterator(it, this);
		}

		Pred pred() const
		{
			return pred_;
		}

	private:
		Pred pred_;
	};

	// selection_t

	template<class Table, class Pred>
		struct selection_t
	{
		typedef sel_t<Table, Pred> type;
	};

	// selection

	template<class Table, class Pred>
		typename selection_t<Table, Pred>::type selection(const Table& t, Pred p)
	{
		return typename selection_t<Table, Pred>::type(t, p);
	}

}}

#endif//BOOST_RTL_SELECTION_HPP_INCLUDED
