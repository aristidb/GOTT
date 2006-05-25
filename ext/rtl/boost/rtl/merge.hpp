// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_MERGE_HPP_INCLUDED
#define BOOST_RTL_MERGE_HPP_INCLUDED

#include <boost/rtl/binary.hpp>
#include <boost/rtl/list.hpp>
#include <boost/rtl/keys.hpp>
#include <boost/mpl/for_each.hpp>

namespace boost { namespace rel {

	template<class Table1, class Table2, int PrefixSize> class mg_t;

	template <class Table1, class Table2, int PrefixSize>
		struct merge_helper
	{
		typedef typename list_prefix
		<
			typename Table2::sort_list,
			PrefixSize
		>::type left_right_list;

		typedef typename concat
		<
			typename Table1::field_list,
			typename list_difference
			<
				typename Table2::field_list,
				left_right_list
			>::type
		>::type field_list;

		typedef typename concat
		<
			typename Table1::sort_list,
			typename list_suffix<typename Table2::sort_list,PrefixSize>::type
		>::type sort_list;

		typedef typename list_difference
		<
			typename Table2::field_list,
			left_right_list
		>::type right_right_list;

		typedef typename create_pairs
		<
			typename Table1::sort_list,
			typename Table2::sort_list,
			PrefixSize
		>::type common_pairs;

		template<class SubTuple>
			struct projections
		{
			typedef row
			<
				typename list_intersection
				<
					typename Table1::field_list,
					typename SubTuple::field_list
				>::type
			> part1;

			typedef typename full_sublist_conversion
			<
				common_pairs,
				typename SubTuple::field_list   
			>::type left_group;

			typedef typename list_intersection
			<
				typename Table2::field_list,
				typename SubTuple::field_list				
			>::type right_group;

			typedef row
			<
				typename list_union
				<
					left_group,
					right_group
				>::type
			> part2;

			typedef typename list_prefix
			<
				common_pairs,
				rel::size<typename SubTuple::field_list>::value
			>::type subtuple_pairs;

			static part1 projection1(const SubTuple& sub)
			{
				part1 temp;
				copy_fields<typename part1::field_list>(sub,temp);
				return temp;
			}

			static part2 projection2(const SubTuple& sub)
			{
				part2 temp;
				boost::mpl::for_each<subtuple_pairs>(
					copy_different_fields<SubTuple,part2>(sub,temp)); 
				copy_fields<right_group>(sub,temp);
				return temp;
			}
		};
	};


	template<class Table1, class Table2, unsigned PrefixSize> 
		class merge_iterator : public binary_iterator<
			merge_iterator<Table1,Table2,PrefixSize>, 
			mg_t<Table1,Table2,PrefixSize> 
			>
	{
		typedef binary_iterator<
			merge_iterator<Table1,Table2,PrefixSize>, 
			mg_t<Table1,Table2,PrefixSize> 
			> base_type;

		friend class binary_iterator<merge_iterator<Table1,Table2,PrefixSize>, mg_t<Table1,Table2,PrefixSize> >;
	public:
		BOOST_RTL_PULL_BINARY_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr1_;
        using base_type::itr2_;
        using base_type::relation_;

		typedef typename merge_helper<Table1, Table2, PrefixSize>::left_right_list left_right_list;
		typedef typename merge_helper<Table1, Table2, PrefixSize>::right_right_list right_right_list;
		merge_iterator(const arg1_iterator& itr1, const arg2_iterator& itr2, const relation_type* o)
			: base_type(itr1,itr2,o)
		{}

		merge_iterator()
		{}
		
		void move_to_prev()
		{
			const arg1_iterator b1 = this->relation()->arg1_.begin();
			const arg2_iterator b2 = this->relation()->arg2_.begin();
			const arg2_iterator e2 = this->relation()->arg2_.end();
			int cmp;
			while(((cmp = itr_comp(itr1_,itr2_)) != 0) && (itr1_ != b1) && (itr2_ != b2))
			{
				if (cmp < 0)
				{
					--itr2_;
				}
				else
				{
					--itr1_;
				}
				setRestartLow(e2);
			}
			if (cmp==0)
			{
				setRestartHigh(itr2_);
			}
			else
			{
				assert(0);
			}
		}

		void move_to_next()
		{
			const arg1_iterator e1 = this->relation()->arg1_.end();
			const arg2_iterator e2 = this->relation()->arg2_.end();
			int cmp;

			while((itr1_ != e1) && (itr2_ != e2) && ((cmp = itr_comp(itr1_,itr2_))!=0))
			{
				if (cmp<0)
				{
					++itr1_;
				}
				else
				{
					++itr2_;
				}
				setRestartHigh(e2);
			}
			if ((itr1_ != e1) && (itr2_ != e2))
			{
				setRestartLow(itr2_);
			}
			else
			{
				itr1_ = e1;
				itr2_ = e2;
				setRestartLow(e2);
			}
		}

		void setRestartLow(arg2_iterator i2)
		{
			restartLow_ = i2;
		}

		void setRestartHigh(arg2_iterator i2)
		{
			restartHigh_ = i2;
		}
	private:
		struct get1
		{
			get1(const arg1_iterator& i1, const arg2_iterator& i2) : i1_(i1), i2_(i2) {}
			template<class Column>
				typename Column::type get(const Column& c) const
			{
				return i1_.get(c);
			}
			const arg1_iterator& i1_;
			const arg2_iterator& i2_;
		};

		struct get2
		{
			get2(const arg1_iterator& i1, const arg2_iterator& i2) : i1_(i1), i2_(i2) {}
			template<class Column>
				typename Column::type get(const Column& c) const
			{
				return i2_.get(c);
			}
			const arg1_iterator& i1_;
			const arg2_iterator& i2_;
		};

	public:
		template<class Column>
			typename Column::type
		get_internal(const Column& c) const 
		{
			typedef typename boost::mpl::if_c
			<
				list_contains<typename Table1::field_list, Column>::value,
				get1,
				get2
			>::type result;
			return result(itr1_,itr2_).get(c);
		}

		static bool itrEqual_11(const typename Table1::const_iterator& i1, const typename Table1::const_iterator& i2)
		{
			typedef typename list_prefix<typename Table1::sort_list,PrefixSize>::type prefix;
			return compare_iterators<prefix>(i1,i2)==0;
		}

		static int itr_comp(const typename Table1::const_iterator& i1, const typename Table2::const_iterator& i2)
		{
			typedef typename create_pairs
			<
				typename Table1::sort_list,
				typename Table2::sort_list,
				PrefixSize
			>::type prefix;
			return compare_different_iterators<prefix>(i1,i2);
		}

		static bool itrEqual_22(const typename Table2::const_iterator& i1, const typename Table2::const_iterator& i2)
		{
			typedef typename list_prefix<typename Table2::sort_list,PrefixSize>::type prefix;
			return compare_iterators<prefix>(i1,i2)==0;
		}

		// pre-begin => true
		// post-begin => 
		//              ((itr1' == end) && (itr2' == end) && (no-match-exists(begin1,end1,begin2,end2)) ||
		//				(*itr1' == *itr2') && !(match-exists-incl(begin1,itr1',begin2,itr2'))
		//
		// pre-end => true
		// post-end => (itr1 == end1) && (itr2 == end2)
		//
		// invariant => ((itr1 == end1) && (itr2 == end2)) || (*itr1 == *itr2)
		//
		// pre-increment => (itr1 != end1) && (itr2 != end2) && (*itr1 == *itr2)
		// post-increment => invariant && !(match-exists-excl(itr1,itr1',itr2,itr2'))
		//
		// pre-move_to_next => (
		//
		// pre-decrement => invariant && (match-exists-incl(begin1,itr1,begin2,itr2))
		// post-decrement => (*itr1' == *itr2') && !(match-exits-excl(itr1,itr1',itr2,itr2'))
		//
		// bounds-related-invariant => 
		//		((itr1 == end1) && (itr2 == end2)) && 
		//			(restartLow_ == end2) && (restartHigh_ == end2)
		//		|| (*itr1 == *itr2) &&
		//			((restartLow_ == end2) && (restartHigh_ == end2) && 
		// 

		template<class Iter>
			static Iter prev(Iter i)
		{
			return --i;
		}

		template<class Iter>
			static Iter next(Iter i)
		{
			return ++i;
		}

		arg2_iterator
		getRestartHigh() const          
		{
			return restartHigh_;
		}

		arg2_iterator
		getRestartLow() const
		{
			return restartLow_;
		}

		void 
		increment()
		{			
			const arg1_iterator e1 = this->relation()->arg1_.end();
			const arg2_iterator e2 = this->relation()->arg2_.end();
			const arg2_iterator n2 = next(itr2_);
			if ((n2 == e2) || (!itrEqual_22(itr2_,n2))) 
			{
				itr2_ = n2;
				setRestartHigh(itr2_);
				if (next(itr1_) == e1)
				{	
					itr1_ = e1;
					itr2_ = e2;
					setRestartHigh(itr2_);
					setRestartLow(itr2_);
				}
				else
				{
					const arg1_iterator n1 = next(itr1_);
					if(itrEqual_11(n1,itr1_))
					{
						itr1_ = n1;
						itr2_ = getRestartLow(); 
					}
					else
					{
						move_to_next();
					}
				}
			}
			else
			{
				++itr2_;
			}
		}

		void decrement()
		{
			if(itr1_ == relation_->arg1_.end())
			{
				--itr1_;
				--itr2_;
				if(itr_comp(itr1_,itr2_)==0)
				{
					setRestartHigh(relation_->arg2_.end());
				}
				else
				{
					move_to_prev();
				}
			}
			else
			{
				const arg2_iterator p2 = prev(itr2_);
				if(itrEqual_22(p2,itr2_))
				{
					itr2_ = p2;
				}
				else
				{
					const arg1_iterator p1 = prev(itr1_); 
					if(itrEqual_11(p1,itr1_))
					{
						itr1_ = p1;
						setRestartLow(itr2_);
						itr2_ = getRestartHigh();
						--itr2_;
					}
					else
					{
						itr1_ = p1;
						--itr2_;
						move_to_prev();
					}
				}
			}
		}


	private:
		arg2_iterator restartLow_;
		arg2_iterator restartHigh_;
	};

	template <class Table1, class Table2, int PrefixSize>
		struct mg_base : boost::mpl::identity<binary_operator<
			Table1,
			Table2,
			typename merge_helper<Table1,Table2,PrefixSize>::field_list,
			typename merge_helper<Table1,Table2,PrefixSize>::sort_list,
			merge_iterator<Table1,Table2,PrefixSize>
		> > {};

    struct merge_tag {};

	template <class Table1, class Table2, int PrefixSize>
		class mg_t : public mg_base<Table1, Table2, PrefixSize>::type
	{
		typedef typename mg_base<Table1, Table2, PrefixSize>::type base_type;
		friend class merge_iterator<Table1,Table2,PrefixSize>;
	
	public:
        typedef merge_tag tag;

		BOOST_RTL_PULL_BINARY_OPERATOR_TYPEDEFS(base_type);
        using base_type::arg1_;
        using base_type::arg2_;

//		enum {prefix_size = PrefixSize};
		BOOST_STATIC_CONSTANT(int, prefix_size=PrefixSize);
		mg_t(const Table1& x, const Table2& y) : base_type(x,y) {};

		typedef typename merge_keys
		<
			typename Table1::keys,
			typename Table2::keys,
			typename list_prefix
			<
				typename Table2::sort_list,
				prefix_size
			>::type
		>::type keys;

		const_iterator begin() const
		{
			const typename Table1::const_iterator b1 = arg1_.begin();
			const typename Table2::const_iterator b2 = arg2_.begin();
			const typename Table1::const_iterator e1 = arg1_.end();
			const typename Table2::const_iterator e2 = arg2_.end();
			if ((b1 == e1) || (b2 == e2))
			{
				const_iterator temp(e1, e2, this);
				temp.setRestartLow(e2);
				temp.setRestartHigh(e2);
				return temp;
			}
			else
			{
				const_iterator temp(b1, b2, this);
				if(const_iterator::itr_comp(b1,b2)!=0)
				{
					temp.setRestartLow(e2);
					temp.setRestartHigh(e2);
					temp.move_to_next();
				}
				else
				{
					temp.setRestartLow(b2);
				}
				return temp;
			}
		}

		const_iterator end() const 
		{
			const typename Table2::const_iterator e2 = arg2_.end();
			const_iterator temp(arg1_.end(), e2, this);
			temp.setRestartLow(e2);
			temp.setRestartHigh(e2);
			return temp;			
		}
		
		template<class SubTuple>
			range_t<const_iterator> equal_range(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			typedef mg_t
				<
					range_t<typename arg1_type::const_iterator>,
					range_t<typename arg2_type::const_iterator>,
					prefix_size
				> equivalent_operator_type;

			typedef typename merge_helper<Table1,Table2,prefix_size>::template projections<SubTuple> proj;

			typename proj::part1 tup1(proj::projection1(sub));
			typename proj::part2 tup2(proj::projection2(sub));

			equivalent_operator_type m(
				arg1_.equal_range(tup1),
				arg2_.equal_range(tup2)
			);
			
			typename equivalent_operator_type::const_iterator mb(m.begin());
			
			const_iterator begin(mb.base1().base(), mb.base2().base(), this);
			//... estabishing restarts...
			begin.setRestartLow(mb.getRestartLow().base());
			begin.setRestartHigh(mb.getRestartHigh().base());

			typename equivalent_operator_type::const_iterator me(m.end());
			const_iterator end;
			
			if (m.end() == m.begin()) end = begin;
			else
			{
				typename equivalent_operator_type::const_iterator last = m.end();
				--last; // points to the last tuple in equivalent operator
				end = const_iterator(last.base1().base(), last.base2().base(), this);
				//...establish restarts...
				end.setRestartLow(last.getRestartLow().base());
				end.setRestartHigh(last.getRestartHigh().base());
				++end;
			}

			return range_t<const_iterator>(begin,end);
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);
			typedef mg_t
				<
					range_t<typename arg1_type::const_iterator>,
					range_t<typename arg2_type::const_iterator>,
					prefix_size
				> equivalent_operator_type;

			typedef typename merge_helper<Table1,Table2,prefix_size>::template projections<SubTuple> proj;

			typename proj::part1 tup1(proj::projection1(sub));
			typename proj::part2 tup2(proj::projection2(sub));

			range_t<typename arg1_type::const_iterator> r1(arg1_.lower_bound(tup1),arg1_.end());
			range_t<typename arg2_type::const_iterator> r2(arg2_.lower_bound(tup2),arg2_.end());

			equivalent_operator_type m(r1,r2);
			
			return const_iterator(m.begin().base1().base(), m.begin().base2().base(), this);
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& sub) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);
			typedef mg_t
				<
					range_t<typename arg1_type::const_iterator>,
					range_t<typename arg2_type::const_iterator>,
					prefix_size
				> equivalent_operator_type;

			typedef typename merge_helper<Table1,Table2,prefix_size>::template projections<SubTuple> proj;

			typename proj::part1 tup1(proj::projection1(sub));
			typename proj::part2 tup2(proj::projection2(sub));

			range_t<typename arg1_type::const_iterator> r1(arg1_.begin(),arg1_.upper_bound(tup1));
			range_t<typename arg2_type::const_iterator> r2(arg2_.begin(),arg2_.upper_bound(tup2));

			equivalent_operator_type m(r1,r2);

			const_iterator begin(m.begin().base1().base(), m.begin().base2().base(), this);
			const_iterator end;
			if (m.end() == m.begin()) end = begin;
			else
			{
				typename equivalent_operator_type::const_iterator last = m.end();
				--last; // points to the last tuple in equivalent operator
				end = const_iterator(last.base1().base(), last.base2().base(), this);
				++end;
			}
			return end;
		}
	};

	template <class Table1, class Table2, int PrefixSize>
		class merge_t
	{
	public:
		typedef mg_t<Table1,Table2,PrefixSize> type;
	};

	template<int PrefixSize, class Table1, class Table2>
		mg_t<Table1, Table2, PrefixSize> merge(const Table1& t1, const Table2& t2)
	{
		return mg_t<Table1, Table2, PrefixSize>(t1,t2);
	}

}}

#endif
