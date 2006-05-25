// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_ISECTION_HPP_INCLUDED
#define BOOST_RTL_ISECTION_HPP_INCLUDED

#include <boost/rtl/binary.hpp>
#include <boost/rtl/range.hpp>
#include <boost/rtl/list.hpp>
#include <boost/rtl/keys.hpp>

namespace boost { namespace rel {

	template<class Table1, class Table2, class IsPolicy> class is_t;

	// itr2_ always contains end of second operand, only if match found, it is set to matched pair.
	template<class Table1, class Table2, class IsPolicy>
		class isection_iterator 
		:	public binary_iterator
			<
				isection_iterator<Table1,Table2,IsPolicy>,
				is_t<Table1,Table2,IsPolicy>
			>
	{
		typedef binary_iterator
		<
			isection_iterator<Table1,Table2,IsPolicy>,
			is_t<Table1,Table2,IsPolicy>
		> base_type;

		friend class iterator_root<isection_iterator,is_t<Table1,Table2,IsPolicy> >;
		friend class is_t<Table1,Table2,IsPolicy>;
	public:
		BOOST_RTL_PULL_BINARY_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr1_;
        using base_type::itr2_;

		isection_iterator() {}

	private:
		struct get1
		{
			get1(const arg1_iterator& i1, const arg2_iterator&) 
				: i1_(i1)
			{}
			template<class Column>
				typename Column::type get(const Column& c) const
			{
				return i1_.get(c);
			}
			const arg1_iterator& i1_;
		};

		struct get2
		{
			get2(const arg1_iterator&, const arg2_iterator& i2) 
				: i2_(i2) {}
			template<class Column>
				typename Column::type get(const Column& c) const
			{
				return i2_.get(c);
			}
			const arg2_iterator& i2_;
		};
	public:
		template<class Column>
			typename Column::type
		get_internal(const Column& c) const 
		{
			typedef typename boost::mpl::if_c
			<
				IsPolicy::second_iterator_used,
				get2,
				get1
			>::type result;
			return result(itr1_,itr2_).get(c);
		}

		isection_iterator(const arg1_iterator& itr1, const is_t<Table1,Table2,IsPolicy>* op) 
			: base_type(itr1,op->arg2_.end(),op)	  // second iterator by default set to end()
		{
		}

	private:
		static int compare_sides(const arg1_iterator& i1, const arg2_iterator& i2) 
		{
			return compare_iterators<typename relation_type::sort_list>(i1,i2);
		}

		arg2_iterator get_matching() const
		{
			row<typename relation_type::sort_list> temp;
			copy_fields<typename relation_type::sort_list>(itr1_, temp);
			return this->relation()->arg2_.lower_bound(temp);
		}

		void move_to_next()
		{
			arg1_iterator e1 = this->relation()->arg1_.end();
			arg2_iterator e2 = this->relation()->arg2_.end();
			
			do
			{
				if(itr1_ == e1)
				{
					itr2_ = e2;
					break;
				}
				itr2_ = get_matching(); // assumes itr1_ != e1
				if (itr2_ == e2)
				{
					if (IsPolicy::negate_matching)
					{
						break;
					}
				}
				else
				{
					if (IsPolicy::negate_matching ^ (compare_sides(itr1_,itr2_) == 0))
					{
						break;
					}
				}
				++itr1_;
			} while(true);
		}


		void increment()
		{
			++itr1_;
			move_to_next();
		}

		void decrement()
		{
			arg2_iterator e2 = this->relation()->arg2_.end();
			do
			{
				--itr1_;
				itr2_ = get_matching(); // assumes itr1_ != e1
				if (itr2_ == e2)
				{
					if(IsPolicy::negate_matching)
					{
						break;
					}
				}
				else
				{
					if (IsPolicy::negate_matching ^ (compare_sides(itr1_,itr2_) == 0))
					{
						break;
					}
				}
			} while(true);
		}
	};

	template <class Table1, class Table2, class IsPolicy> 
		struct is_base : boost::mpl::identity
		<
			binary_operator
			<
				Table1,
				Table2,
				typename Table1::field_list,
				typename Table1::sort_list,
				isection_iterator<Table1,Table2,IsPolicy>
			> 
		> 
	{};

	struct is_pl
	{
		enum {negate_matching = false};
		enum {second_iterator_used = false};
	};

	struct rs_pl
	{
		enum {negate_matching = false};
		enum {second_iterator_used = true};
	};

	struct df_pl
	{
		enum {negate_matching = true};
		enum {second_iterator_used = false};
	};

	template<class Keys1, class Keys2, class Policy>
		struct isection_keys
	{
		typedef typename boost::mpl::eval_if
		<
			boost::is_same<Policy,is_pl>,
			intersection_keys<Keys1,Keys2>,
			typename boost::mpl::eval_if
			<
				boost::is_same<Policy,rs_pl>,
				intersection_keys<Keys2,Keys1>,
				difference_keys<Keys1>
			>
		>::type type;
	};

    template<class IsPolicy> struct isection_tag_ {};

    typedef isection_tag_<is_pl> isection_tag; 
    typedef isection_tag_<rs_pl> rsection_tag; 
    typedef isection_tag_<df_pl> diff_tag; 

	template <class Table1, class Table2, class IsPolicy> 
		class is_t : public is_base<Table1, Table2, IsPolicy>::type
	{
		typedef typename is_base<Table1, Table2, IsPolicy>::type base_type;
		friend class isection_iterator<Table1, Table2,IsPolicy>; 

	public:
        typedef isection_tag_<IsPolicy> tag;

		enum {is_sizeable = false};
		typedef IsPolicy is_policy;

		BOOST_RTL_PULL_BINARY_OPERATOR_TYPEDEFS(base_type);
        using base_type::arg1_;
        using base_type::arg2_;

		typedef typename isection_keys
		<
			typename Table1::keys,
			typename Table2::keys,
			is_policy
		>::type keys;

		is_t(const Table1& t1, const Table2& t2)
			: base_type(t1,t2)
		{
		}
		
		const_iterator begin() const 
		{
			const_iterator temp(arg1_.begin(), this);
			temp.move_to_next();
			return temp;
		}

		const_iterator end() const
		{
			return const_iterator(arg1_.end(), this);			
		}

		template<class SubTuple>
			range_t<const_iterator> equal_range(const SubTuple& s) const
		{
			range_t<typename const_iterator::arg1_iterator> temp(arg1_.equal_range(s));
			const_iterator low(temp.begin().base(),this);
			low.move_to_next();
			const_iterator high(temp.end().base(),this);
			high.move_to_next();
			return range_t<const_iterator>(low,high);
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& s) const
		{
			const_iterator temp(arg1_.lower_bound(s),this);
			temp.move_to_next();
			return temp;
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& s) const
		{
			const_iterator temp(arg1_.upper_bound(s),this);
			temp.move_to_next();
			return temp;
		}
	};


	template<class Table1, class Table2>
		struct isection_t
	{
		typedef is_t<Table1, Table2, is_pl> type;
	};

	template<class Table1, class Table2>
		struct rsection_t
	{
		typedef is_t<Table1, Table2, rs_pl> type;
	};

	template<class Table1, class Table2>
		struct diff_t
	{
		typedef is_t<Table1, Table2, df_pl> type;
	};


	template<class Table1, class Table2>
	typename isection_t<Table1,Table2>::type 
	isection(const Table1& t1, const Table2& t2)
	{
		return typename isection_t<Table1,Table2>::type(t1,t2);
	}

	template<class Table1, class Table2>
	typename rsection_t<Table1,Table2>::type 
	rsection(const Table1& t1, const Table2& t2)
	{
		return typename rsection_t<Table1,Table2>::type(t1,t2);
	}

	template<class Table1, class Table2>
	typename diff_t<Table1,Table2>::type 
	diff(const Table1& t1, const Table2& t2)
	{
		return typename diff_t<Table1,Table2>::type(t1,t2);
	}
}}

#endif//BOOST_RTL_ISECTION_HPP_INCLUDED
