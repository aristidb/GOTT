// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_RANGE_HPP_INCLUDED
#define BOOST_RTL_RANGE_HPP_INCLUDED

#include <boost/rtl/tuple.hpp>
#include <boost/rtl/iterator_root.hpp>
#include <boost/rtl/list.hpp>

namespace boost { namespace rel {

	template<class Iter, class Op> class range_iterator
		: public iterator_root<range_iterator<Iter, Op>, Op>
	{
		friend class iterator_root<range_iterator<Iter, Op>, Op>;

	public:
		typedef typename range_iterator::value_type value_type;
		typedef typename range_iterator::relation_type relation_type;
		typedef typename range_iterator::base_type base_type;

		range_iterator(const Iter& itr, const relation_type* o) 
			: base_type(o) 
			, itr_(itr)
		{}

		range_iterator() 
			: itr_(Iter())
		{}

		Iter base() const 
		{
			return itr_;
		}

		bool operator==(const range_iterator& other) const
		{
			return 
				itr_ == other.itr_ &&
				this->relation_ == other.relation_;
		}

		template<class Column>
			typename Column::type get_internal(const Column& c) const 
		{
			return itr_.get(c);
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
	private:
		Iter itr_;
	};

	template<class Iter> class range_t;

	template<class Policy> class basic_range
	{
	public:
		typedef typename Policy::iterator::relation_type base_relation_type;
		typedef typename Policy::iterator::value_type value_type;
		typedef typename Policy::iterator::value_type::field_list field_list;
		typedef typename Policy::iterator::relation_type::sort_list sort_list;
		typedef typename Policy::iterator::relation_type::keys keys;
		typedef range_iterator<typename Policy::iterator, typename Policy::relation> const_iterator;

	public:
		basic_range(const Policy& policy) 
			: policy_(policy) 
		{}
	public:
		const_iterator begin() const 
		{
			return const_iterator(policy_.begin(), static_cast<const typename Policy::relation*>(this));
		}
		const_iterator end() const 
		{
			return const_iterator(policy_.end(), static_cast<const typename Policy::relation*>(this));
		}
		template<class SubTuple>
			range_t<const_iterator> equal_range(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			if (policy_.begin() == policy_.end())
			{
				return range(begin(), end());
			}

			//	+----+----+----+----+
			//  |    | -1 | 0  |  1 | compare to begin
			//	+----+----+----+----+
			//  | -1 | bb | bu | r  |
			//	+----+----+----+----+
			//  |  0 | NA | be | le |
			//	+----+----+----+----+
			//  |  1 | NA | NA | ee |
			//	+----+----+----+----+
			//
			//
			typedef typename list_intersection<sort_list,typename SubTuple::field_list>::type compare_list;

			const_iterator b;
			const_iterator e;
			int lower_bound_situation = compare_tuples<compare_list>(sub,*policy_.begin());
			int upper_bound_situation = compare_tuples<compare_list>(sub,*(prev(policy_.end())));
			if (lower_bound_situation == -1)
			{
				return range(begin(), begin());
			}	
			if (upper_bound_situation == 1)
			{
				return range(end(), end());
			}
			if ((lower_bound_situation == 0) && (upper_bound_situation == -1))
			{
				return range(
					begin(), 
					const_iterator(base_operator().upper_bound(sub), static_cast<const typename Policy::relation*>(this)));
			}
			if ((lower_bound_situation == 1) && (upper_bound_situation == -1))
			{
				range_t<typename Policy::iterator> r = base_operator().equal_range(sub);
				return range(
					const_iterator(r.begin().base(), static_cast<const typename Policy::relation*>(this)), 
					const_iterator(r.end().base(), static_cast<const typename Policy::relation*>(this)));
			}
			if ((lower_bound_situation == 1) && (upper_bound_situation == 0))
			{
				return range(
					const_iterator(base_operator().lower_bound(sub), static_cast<const typename Policy::relation*>(this)),
					end());
			}
			assert((lower_bound_situation == 0) && (upper_bound_situation == 0));
			return range(begin(), end());
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			typedef typename list_intersection<sort_list,typename SubTuple::field_list>::type compare_list;

			int lower_bound_situation = compare_tuples<compare_list>(sub,*policy_.begin());
			if ((lower_bound_situation == -1) || (lower_bound_situation == 0))
			{
				return begin();
			}	
			else
			{
				int upper_bound_situation = compare_tuples<compare_list>(sub,*(prev(policy_.end())));
				if ((upper_bound_situation == -1) || (upper_bound_situation == 0))
				{
					return const_iterator(base_operator().lower_bound(sub), static_cast<const typename Policy::relation*>(this));
				}
				else
				{
					return end();
				}
			}

		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			typedef typename list_intersection<sort_list,typename SubTuple::field_list>::type compare_list;

			int upper_bound_situation = compare_tuples<compare_list>(sub,*(prev(policy_.end())));
			if (upper_bound_situation == -1)
			{
				int lower_bound_situation = compare_tuples<compare_list>(sub,*policy_.begin());
				if ((lower_bound_situation == 0) || (lower_bound_situation == 1))
				{
					return const_iterator(base_operator().upper_bound(sub), static_cast<const typename Policy::relation*>(this));
				}
				else
				{
					return begin();
				}
			}
			else
			{
				return end();
			}
		}
	private:
		static 
		typename Policy::iterator prev(typename Policy::iterator i)
		{
			return --i;
		}

		const typename Policy::iterator::relation_type& base_operator() const 
		{
			const typename Policy::iterator::relation_type* res = policy_.begin().relation();
			assert(res != 0);
			return *res;
		}

	protected:
		Policy policy_;
	};

	template<class Iter>
		class range_policy
	{
	public:
		typedef Iter iterator;
		typedef range_t<Iter> relation;

		range_policy(Iter begin, Iter end) 
			: begin_(begin), end_(end) 
		{
			assert(begin.relation() == end.relation()); 
		}
		Iter begin() const {return begin_;}
		Iter end() const {return end_;}

	private:
		Iter begin_;
		Iter end_;
	};

	template<class Iter> class range_t 
		: public basic_range<range_policy<Iter> >
	{
	public:
		range_t(Iter begin, Iter end) 
			: basic_range<range_policy<Iter> >(range_policy<Iter>(begin, end)) 
		{}
	};

	template<class Iter>
		range_t<Iter> range(Iter begin, Iter end)
	{
		return range_t<Iter>(begin, end);
	}

	template<class Iter> 
		range_t<Iter> range(Iter i)
	{
		Iter j(i++);
		return range(j, i);
	}
}}

#endif//BOOST_RTL_RANGE_HPP_INCLUDED
