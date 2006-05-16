// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_VECTOR_IMPL_HPP_INCLUDED
#define BOOST_RTL_VECTOR_IMPL_HPP_INCLUDED

#include <boost/rtl/tuple.hpp>
#include <boost/rtl/range.hpp>
#include <algorithm>
#include <vector>
#include <boost/rtl/list.hpp>

namespace boost { namespace rel {

	// in memory implementation optimized for rare inserts and deletes. Updates are OK
	// Inserts are based on sort/merge
	// Deletes uses tuple_base flag

	inline 
	bool 
	isDeleted(const tuple_base& t) {return t.isDeleted();}

	template<class Tuple, class SortList> class sorted_vector  
	{
	public:
		typedef std::vector<Tuple> data;
		sorted_vector() 
			: totalRecordsToRemove_(0)
			, firstUnsorted_(0)
		{}

	public:
		typedef typename data::const_iterator data_iterator;
		typedef std::pair<data_iterator,data_iterator> data_range;

		void 
		reserve(std::size_t new_size)
		{
			data_.reserve(new_size);
		}

		std::size_t size() const 
		{ 
			return data_.size();
		}

		data_iterator begin() const
		{
			sort();
			return data_.begin();
		}

		data_iterator end() const
		{
			sort();
			return data_.end();
		}

		template<class Sub, class Size>
			data_range equal_range(const Sub& sub, const Size&) const
		{
			sort();
			equal_range_cmp<typename list_prefix<SortList, Size::value>::type> pr;
			return std::equal_range(data_.begin(), data_.end(), sub, pr);
		}

		template<class Sub, class Size>
			data_iterator lower_bound(const Sub& sub, const Size&) const
		{
			sort();
			equal_range_cmp<typename list_prefix<SortList, Size::value>::type> pr;
			return std::lower_bound(data_.begin(), data_.end(), sub, pr);
		}

		template<class Sub, class Size>
			data_iterator upper_bound(const Sub& sub, const Size&) const
		{
			sort();
			equal_range_cmp<typename list_prefix<SortList, Size::value>::type> pr;
			return std::upper_bound(data_.begin(), data_.end(), sub, pr);
		}

		void insert(const Tuple& t) 
		{
			data_.push_back(t);
		}

		void remove(data_iterator itr)
		{
			++totalRecordsToRemove_;
			(*itr).setDeleted(true);
		}

		void clear()
		{
//			data temp;  // create empty
//			data_.swap(temp);
			data_.clear();
			totalRecordsToRemove_ = 0;
			firstUnsorted_ = 0;
		};

		void update(data_iterator itr, const Tuple& t)
		{
			data_iterator b(data_.begin());
			size_t dist = std::distance(b,itr);
			typename data::iterator location(data_.begin());
			std::advance(location,dist);
			*location = t;
		}

	private:
		void sort() const // to be accessible from const methods, relies on mutable modifiers
		{
			if (firstUnsorted_ != data_.size())
			{
				typename data::iterator middle(data_.begin());
				std::advance(middle, firstUnsorted_);
				std::sort(middle, data_.end(), equal_range_cmp<SortList>());
				std::inplace_merge(data_.begin(),middle,data_.end(),equal_range_cmp<SortList>());
			}
			if (totalRecordsToRemove_ != 0)
			{
				data_.erase(std::remove_if(data_.begin(),data_.end(),isDeleted),data_.end());
				totalRecordsToRemove_ = 0;
			}
			firstUnsorted_ = data_.size();
		}

	private:
		mutable size_t totalRecordsToRemove_;
		mutable size_t firstUnsorted_;
		mutable data data_;
	};

	struct sorted_strategy 
	{
		template<class Tuple, class SortList>
			struct apply : boost::mpl::identity<sorted_vector<Tuple, SortList> >
		{};
	};
}}

#endif

