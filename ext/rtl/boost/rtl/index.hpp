// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_INDEX_HPP_INCLUDED
#define BOOST_RTL_INDEX_HPP_INCLUDED

#include <boost/rtl/tuple.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/iterator_root.hpp>
#include <boost/rtl/identifiable.hpp>
#include <boost/rtl/list.hpp>
#include <boost/rtl/unary.hpp>

#define BOOST_RTL_PULL_INDEX_TYPEDEFS(Base)			\
	BOOST_RTL_PULL_UNARY_OPERATOR_TYPEDEFS(Base)    \
	typedef typename Base::strategy_type strategy_type; 		\
	typedef typename Base::index_tuple index_tuple;

namespace boost { namespace rel {

	template <class Derived, class Relation> class index_iterator
		: public iterator_root<Derived, Relation>
	{
		friend class iterator_root<Derived, Relation>;
        typedef iterator_root<Derived, Relation> base_type;

	public:
		typedef typename Relation::idx::const_iterator idx_iterator;
		using base_type::relation_type;
        using base_type::relation_;

		idx_iterator base() const 
		{
			return itr_;
		}
	public:
		bool operator== (const Derived& other) const 
		{
			return 
				itr_ == other.itr_ &&
				relation_ == other.relation_;
		}
	private:
		void increment()
		{
			++itr_;
		}
		void decrement()
		{
			--itr_;
		}
	protected:
		typedef index_iterator index_base_type;

		index_iterator(const idx_iterator& itr, const Relation* o) 
			: base_type(o)
			, itr_(itr)
		{}
		index_iterator()
			: base_type(0) 
			, itr_(idx_iterator())
		{}
	protected:
		idx_iterator itr_;
	};

	template
	<
		class Table,				
		class SortList,                        
		class IndexTuple,		
		class Strategy,
		class Iterator,
		class Derived
	>
	class index : public identifiable
	{
		
	public:
		index(const Table& t) 
			: table_(new Table(t))
			, index_()
		{
		}

		typedef IndexTuple index_tuple;
		typedef table
		<
			table_info
			<
				typename IndexTuple::field_list,
				SortList,
				typename projection_keys
				<
					typename Table::keys, 
					typename IndexTuple::field_list
				>::type,
				Strategy
			> 
		> idx;

		typedef Iterator const_iterator;
		typedef typename Table::field_list field_list;
		typedef SortList sort_list;
		typedef row<field_list> value_type;
		typedef index base_type;
		typedef Strategy strategy_type;
		typedef Table arg_type;
		typedef typename Table::keys keys;
		
		std::size_t size() const {return index_.size();}

		const_iterator begin() const
		{
			return const_iterator(index_.begin(),static_cast<const Derived*>(this));
		}

		const_iterator end() const
		{
			return const_iterator(index_.end(),static_cast<const Derived*>(this));
		}
		
		template <class SubTuple>
		range_t<const_iterator> equal_range(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			range_t<typename idx::const_iterator> r(index_.equal_range(sub));
			return range_t<const_iterator>(
				const_iterator(r.begin().base(),static_cast<const Derived*>(this)),
				const_iterator(r.end().base(),static_cast<const Derived*>(this)));
		}

		template<class SubTuple>
		const_iterator lower_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			return const_iterator(index_.lower_bound(sub),static_cast<const Derived*>(this));
		}

		template<class SubTuple>
		const_iterator upper_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			return const_iterator(index_.upper_bound(sub),static_cast<const Derived*>(this));
		}

		Table argument() const
		{
			return *table_;
		}

		idx internal_table()
		{
			return index_;
		}

		template<class F>
			void traverse_args(F f) const
		{
			traverse(argument(), f);
		}

	protected:
		boost::shared_ptr<Table> table_;
		idx index_;
	};

}}
#endif//BOOST_RTL_INDEX_HPP_INCLUDED
