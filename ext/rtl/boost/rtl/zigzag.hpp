// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_ZIGZAG_HPP_INCLUDED
#define BOOST_RTL_ZIGZAG_HPP_INCLUDED

#include <boost/rtl/binary.hpp>
#include <boost/rtl/range.hpp>
#include <boost/rtl/keys.hpp>

namespace boost { namespace rel {

	struct d_pl
	{
		static bool accept(int sign)
		{
			return (sign < 0);
		}
		enum {first_moves_when_equal = true};
	};

	struct i_pl
	{
		static bool accept(int sign)
		{
			return (sign == 0);
		}
		enum {first_moves_when_equal = true};

	};

	struct u_pl
	{
		static bool accept(int sign)
		{
			return (sign != 0);
		}
		enum {first_moves_when_equal = false};
	};

	template<class Keys1, class Keys2, class FieldList, class Policy>
		struct zigzag_keys
	{
		typedef typename boost::mpl::eval_if
		<
			boost::is_same<Policy,u_pl>,
			union_keys<Keys1,Keys2,FieldList>,
			typename boost::mpl::eval_if
			<
				boost::is_same<Policy,d_pl>,
				difference_keys<Keys1>,
				intersection_keys<Keys1,Keys2>
			>
		>::type type;
	};

	// zigzag_iterator

	template<class Table1, class Table2, class Policy> class zigzag;

	template <class Table1, class Table2, class Policy> 
		class zigzag_iterator : public binary_iterator
		<
			zigzag_iterator<Table1, Table2, Policy>, 
			zigzag<Table1, Table2, Policy>
		>
	{
		typedef binary_iterator
		<
			zigzag_iterator<Table1, Table2, Policy>, 
			zigzag<Table1, Table2, Policy>
		> base_type;

		friend class iterator_root
		<
			zigzag_iterator<Table1, Table2, Policy>, 
			zigzag<Table1, Table2, Policy> 
		>;
		friend class zigzag<Table1, Table2, Policy>;

	public:
		BOOST_RTL_PULL_BINARY_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr1_;
        using base_type::itr2_;

	private:
		zigzag_iterator(const arg1_iterator& itr1, const arg2_iterator& itr2, const relation_type* o, int sign) 
			: base_type(itr1, itr2, o)
			, sign_(sign)
		{}
	public:
		zigzag_iterator()
		{}

		template<class Column>
			typename Column::type
		get_internal(const Column& c) const 
		{
			return (sign_ <= 0) 
				? itr1_.get(c) 
				: itr2_.get(c);
		}

		int get_sign() const 
		{
			return sign_;
		}

	private:

		static int compare_sides(const arg1_iterator& i1, const arg2_iterator& i2)
		{
			return compare_iterators<typename Table1::sort_list>(i1,i2);
		}

		void establish_sign(bool is_e1, bool is_e2)
		{
			sign_ = is_e1 ? 1 : (is_e2 ? -1 : compare_sides(itr1_,itr2_));
		}

		void incr(const arg1_iterator& e1, const arg2_iterator& e2)
		{
			bool is_e1 = (itr1_ == e1);
			bool is_e2 = (itr2_ == e2);
			do
			{
				if ((sign_ < 0) || ((sign_ == 0) && Policy::first_moves_when_equal))
				{
					++itr1_;
					is_e1 = (itr1_ == e1);
				}
				else
				{
					++itr2_;
					is_e2 = (itr2_ == e2);
				}
				establish_sign(is_e1,is_e2);
			} while(!((is_e1 && is_e2) || Policy::accept(sign_)));
		}

		void increment()
		{
			arg1_iterator e1 = this->relation()->arg1_.end();
			arg2_iterator e2 = this->relation()->arg2_.end();
			incr(e1,e2);
		}

		void decrement()
		{
			arg1_iterator e1 = this->relation()->arg1_.end();
			arg2_iterator e2 = this->relation()->arg2_.end();
			arg1_iterator b1 = this->relation()->arg1_.begin();
			arg2_iterator b2 = this->relation()->arg2_.begin();
			bool is_e1 = (itr1_ == e1);
			bool is_e2 = (itr2_ == e2);
			do
			{
				if (itr1_ == b1)
				{
					--itr2_;
					is_e2 = false;
				}
				else if (itr2_ == b2)
				{
					--itr1_;
					is_e1 = false;
				}
				else
				{
					arg1_iterator temp1(itr1_);
					arg2_iterator temp2(itr2_);
					--temp1;
					--temp2;
					int result = compare_sides(temp1,temp2);
					if ((result < 0) || ((result == 0) && Policy::first_moves_when_equal))
					{
						itr2_ = temp2;
						is_e2 = false;
					}
					else
					{
						itr1_ = temp1;
						is_e1 = false;
					}
				}
				establish_sign(is_e1,is_e2);
			} while(!Policy::accept(sign_));
		}
		int sign_;
	};

	// zigzag_base

	template <class Table1, class Table2, class Policy>
		struct zigzag_base : boost::mpl::identity
		<
			binary_operator
			<
				Table1,
				Table2,
				typename Table1::field_list,
				typename Table1::sort_list,
				zigzag_iterator<Table1, Table2, Policy>
			> 
		> 
	{};

	// zigzag

    template<class Policy> struct zigzag_tag {};

    typedef zigzag_tag<d_pl> difference_tag;
    typedef zigzag_tag<i_pl> intersection_tag;
    typedef zigzag_tag<u_pl> union_tag;

	template <class Table1, class Table2, class Policy>
		class zigzag : public zigzag_base<Table1, Table2, Policy>::type
	{
		typedef typename zigzag_base<Table1, Table2, Policy>::type base_type;
		friend class zigzag_iterator<Table1, Table2, Policy>;

		BOOST_STATIC_CONSTANT(bool, f1 = (list_equal<typename Table1::field_list, typename Table2::field_list>::value));
		BOOST_STATIC_ASSERT(f1);
		BOOST_STATIC_CONSTANT(bool, f2 = (list_equal<typename Table1::sort_list, typename Table2::sort_list>::value));
		BOOST_STATIC_ASSERT(f2);		

		typedef zigzag
		<
			range_t<typename Table1::const_iterator>, 
			range_t<typename Table2::const_iterator>, 
			Policy
		> equivalent_operator_type;

	public:
        typedef zigzag_tag<Policy> tag;

		BOOST_RTL_PULL_BINARY_OPERATOR_TYPEDEFS(base_type);
        using base_type::arg1_;
        using base_type::arg2_;
		typedef Policy policy_type;

		typedef typename zigzag_keys
		<
			typename Table1::keys,
			typename Table2::keys,
			sort_list,
			policy_type
		>::type keys;

		zigzag(const Table1& x, const Table2& y) 
			: base_type(x, y) 
		{}

		const_iterator begin() const 
		{
			typename arg1_type::const_iterator b1 = arg1_.begin();
			typename arg2_type::const_iterator b2 = arg2_.begin();
			typename arg1_type::const_iterator e1 = arg1_.end();
			typename arg2_type::const_iterator e2 = arg2_.end();
			bool is_e1 = (b1 == e1);
			bool is_e2 = (b2 == e2);
			if (is_e1 && is_e2) return const_iterator(b1,b2,this,1);
			int comp = is_e1 ? 1 : (is_e2 ? -1 : const_iterator::compare_sides(b1,b2));
			const_iterator i(b1,b2,this,comp);
			if (!Policy::accept(comp))
			{
				i.incr(e1,e2);
			}
			return i;
		}

		const_iterator end() const 
		{
			return const_iterator(arg1_.end(), arg2_.end(), this, 1);
		}

		template<class SubTuple>
			range_t<const_iterator> equal_range(const SubTuple& subtuple) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);
			equivalent_operator_type z(arg1_.equal_range(subtuple), arg2_.equal_range(subtuple));
			typename equivalent_operator_type::const_iterator b = z.begin();
			const_iterator begin(b.base1().base(), b.base2().base(), this, b.get_sign());
			const_iterator end;
			if (z.end() == b) end = begin;
			else
			{
				typename equivalent_operator_type::const_iterator last = z.end();
				--last; // points to the last tuple in equivalent operator
				end = const_iterator(last.base1().base(), last.base2().base(), this, last.get_sign());
				++end;
			}
			return range(begin, end);
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& subtuple) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			typename arg1_type::const_iterator l1 = arg1_.lower_bound(subtuple);
			typename arg2_type::const_iterator l2 = arg2_.lower_bound(subtuple);
			typename arg1_type::const_iterator e1 = arg1_.end();
			typename arg2_type::const_iterator e2 = arg2_.end();
			bool is_e1 = (l1 == e1);
			bool is_e2 = (l2 == e2);
			if (is_e1 && is_e2) return const_iterator(l1,l2,this,1);
			int comp = is_e1 ? 1 : (is_e2 ? -1 : const_iterator::compare_sides(l1,l2));
			const_iterator i(l1,l2,this,comp);
			if (!Policy::accept(comp))
			{
				i.incr(e1,e2);
			}
			return i;
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& subtuple) const
		{
			const bool valid = valid_subtuple<SubTuple,sort_list>::value;
			BOOST_STATIC_ASSERT(valid);

			typename arg1_type::const_iterator u1 = arg1_.upper_bound(subtuple);
			typename arg2_type::const_iterator u2 = arg2_.upper_bound(subtuple);
			typename arg1_type::const_iterator e1 = arg1_.end();
			typename arg2_type::const_iterator e2 = arg2_.end();
			bool is_e1 = (u1 == e1);
			bool is_e2 = (u2 == e2);
			if (is_e1 && is_e2) return const_iterator(u1,u2,this,1);
			int comp = is_e1 ? 1 : (is_e2 ? -1 : const_iterator::compare_sides(u1,u2));
			const_iterator i(u1,u2,this,comp);
			if (!Policy::accept(comp))
			{
				i.incr(e1,e2);
			}
			return i;
		}
	};
}}

#endif
