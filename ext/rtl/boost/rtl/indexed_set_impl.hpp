#ifndef BOOST_RTL_INDEXED_SET_HPP_INCLUDED
#define BOOST_RTL_INDEXED_SET_HPP_INCLUDED

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/rtl/table.hpp>

namespace boost { namespace rel {

	template<class SortList>
		struct indexed_set_impl_predicate
	{
		template<class Tuple1, class Tuple2>
			bool operator()(const Tuple1& t1, const Tuple2& t2) const
		{
			return compare_tuples<SortList>(t1, t2) < 0;
		}
	};

	template<class Tuple, class SortList>
		class indexed_set_impl
	{
		typedef typename boost::multi_index::multi_index_container<
			Tuple,
			boost::multi_index::indexed_by<
				boost::multi_index::ordered_unique<
					boost::multi_index::identity<Tuple>, 
					indexed_set_impl_predicate<SortList> 
					> 
				>
			> data_type;
	public:
		typedef typename data_type::iterator data_iterator;

		data_iterator begin() const
		{
			return data_.begin();
		}
		data_iterator end() const
		{
			return data_.end();
		}
		template<class Sub, class Size>
			data_iterator lower_bound(const Sub& sub, const Size&) const
		{
			return data_.lower_bound(sub,
				indexed_set_impl_predicate<typename list_prefix<SortList, Size::value>::type>());
		}
		template<class Sub, class Size>
			data_iterator upper_bound(const Sub& sub, const Size&) const
		{
			return data_.upper_bound(sub,
				indexed_set_impl_predicate<typename list_prefix<SortList, Size::value>::type>());
		}
		template<class Sub, class Size>
			std::pair<data_iterator, data_iterator> equal_range(const Sub& sub, const Size&) const
		{
			return data_.equal_range(sub,
				indexed_set_impl_predicate<typename list_prefix<SortList, Size::value>::type>());
		}
		std::size_t size() const 
		{ 
			return data_.size();
		}
		bool insert(const Tuple& t) 
		{
			return data_.insert(t).second;
		}
		void remove(data_iterator itr)
		{
			data_.erase(itr);
		}
		bool update(data_iterator itr, const Tuple& t)
		{
			// fix to do real in-place update!!!

			if (*itr != t)
			{
				remove(itr);
				insert(t);
			}

			return true;
		}
		void clear()
		{
			data_.clear();
		};
		void reserve(int)
		{}
	private:
		data_type data_;
	};

	struct indexed_set_strategy 
	{
		template<class Tuple, class SortList>
			struct apply : boost::mpl::identity<indexed_set_impl<Tuple, SortList> >
		{};
	};
}}

#endif//BOOST_RTL_INDEXED_SET_HPP_INCLUDED
