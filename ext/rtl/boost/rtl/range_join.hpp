#ifndef BOOST_RTL_RANGE_JOIN_HPP_INCLUDED
#define BOOST_RTL_RANGE_JOIN_HPP_INCLUDED

#include <boost/rtl/binary.hpp>
#include <boost/rtl/range_functors.hpp>
#include <boost/rtl/keys.hpp>
#include <boost/rtl/range.hpp>

namespace boost { namespace rel {

	template<class SubTuple, class Table1> class split
	{
	private:
		enum {len1 = rel::size<typename Table1::sort_list>::value};

		enum {long_subtuple = 
			rel::size<typename SubTuple::field_list>::value > (int)len1};

		typedef typename boost::mpl::if_c<
			long_subtuple,
			typename Table1::sort_list,
			typename SubTuple::field_list>::type range_list1;

		typedef typename boost::mpl::if_c<
			long_subtuple,
			typename list_suffix<typename SubTuple::field_list, (long_subtuple ? len1 : 0)>::type,
			boost::mpl::vector0<> >::type range_list2;

	public:
		typedef row<range_list1> tuple1_t;
		typedef row<range_list2> tuple2_t;

		static tuple1_t tuple1(const SubTuple& sub)
		{
			tuple1_t result;
			copy_fields<range_list1>(sub, result);
			return result;
		}

		static tuple2_t tuple2(const SubTuple& sub)
		{
			tuple2_t result;
			copy_fields<range_list2>(sub, result);
			return result;
		}
	};

	template<class Table1, class Table2, class Begin, class End> class rj_t;

	// iterator
	template<class Table1, class Table2, class Begin, class End>
	class range_join_iterator : public binary_iterator<
		range_join_iterator<Table1, Table2, Begin, End>,
		rj_t<Table1, Table2, Begin, End> >
	{
		friend class rj_t<Table1, Table2, Begin, End>;

		typedef binary_iterator<
            range_join_iterator<Table1, Table2, Begin, End>,
            rj_t<Table1, Table2, Begin, End> 
		> base_type;

	public:
		BOOST_RTL_PULL_BINARY_ITERATOR_TYPEDEFS(base_type);

	private:
		range_join_iterator(
			const typename Table1::const_iterator& it1,
			const typename Table2::const_iterator& it2,
			const relation_type* relation)
			: base_type(it1, it2, relation)
		{}
		void update_range()
		{
			begin_ = this->relation_->begin_(this->itr1_, this->relation_->arg2_);
			end_ = this->relation_->end_(this->itr1_, this->relation_->arg2_);
		}
		bool adjust_forward()
		{
			// returns whether itr1_ was moved 
			bool result = false;
            typename Table1::const_iterator end1 = this->relation_->arg1_.end();

            while(true)
			{
				if (this->itr1_ == end1)
				{
					this->itr2_ = this->relation_->arg2_.end();
					return result;
				}
				update_range();
				if (begin_ != end_)
				{
					this->itr2_ = begin_;
					return result;
				}
				++this->itr1_;
				result = true;
			}
		}
		void adjust_backward()
		{
			while(true)
			{
                update_range();

				if (begin_ != end_)
				{
					this->itr2_ = end_;
					--this->itr2_;
					return;
				}
				else
					--this->itr1_;
			}
		}
	public:
		range_join_iterator()
			: base_type(arg1_iterator(), arg2_iterator(), 0)
		{}
		void increment()
		{
			//assert(invariant());
			if (++this->itr2_ == end_)
			{
				++this->itr1_;
				adjust_forward();
			}
			//assert(invariant());
		}
		void decrement()
		{
			//assert(invariant());
			if (this->itr1_ == this->relation_->arg1_.end())
			{
				--this->itr1_;
				adjust_backward();
			}
			else if (this->itr2_ == begin_)
			{
				--this->itr1_;
				adjust_backward();
			}
			else
				--this->itr2_;
			//assert(invariant());
		}
		template<class Column>
			typename Column::type get_internal(const Column& c) const 
		{
			//assert(invariant());
			return get_internal(c, 
				typename list_contains<typename Table1::field_list, Column>::type()); 
		}
	private:
		template<class Column>
			typename Column::type get_internal(const Column& c, boost::mpl::true_) const 
		{
			return this->itr1_.get_internal(c);
		}
		template<class Column>
			typename Column::type get_internal(const Column& c, boost::mpl::false_) const 
		{
			return this->itr2_.get_internal(c);
		}
		bool invariant() const
		{
			return *this == this->relation_->end() || (
				begin_ == this->relation_->begin_(this->itr1_, this->relation_->arg2_) &&
				end_ == this->relation_->end_(this->itr1_, this->relation_->arg2_));
		}
	private:
		arg2_iterator begin_, end_;
	};

	// rang_join
	template<class Table1, class Table2, class Begin, class End>
	struct range_join_base : boost::mpl::identity<
		binary_operator<
			Table1, Table2,
			typename concat<typename Table1::field_list, typename Table2::field_list>::type,
			typename concat<typename Table1::sort_list, typename Table2::sort_list>::type,
			range_join_iterator<Table1, Table2, Begin, End> 
		>	
	>{};

    struct range_join_tag {};

	template<class Table1, class Table2, class Begin, class End>
	class rj_t : public range_join_base<Table1, Table2, Begin, End>::type
	{
		friend class range_join_iterator<Table1, Table2, Begin, End>;
		typedef typename range_join_base<Table1, Table2, Begin, End>::type base_type;

		typedef boost::mpl::true_ short_subtuple;
		typedef boost::mpl::false_ long_subtuple;

	public:
        typedef range_join_tag tag;

		BOOST_RTL_PULL_BINARY_OPERATOR_TYPEDEFS(base_type);

		typedef typename crossproduct_keys<
			typename Table1::keys,
			typename Table2::keys
		>::type keys;

		typedef Begin begin_type;
		typedef End end_type;

		rj_t(const Table1& t1, const Table2& t2, const Begin& b, const End& e)
			: base_type(t1, t2)
			, begin_(b)
			, end_(e)
		{}
		rj_t(const Table1& t1, const Table2& t2)
			: base_type(t1, t2)
		{}
		begin_type begin_functor() const
		{
			return begin_;
		}
		end_type end_functor() const
		{
			return end_;
		}
		const_iterator begin() const
		{
			const_iterator it(this->arg1_.begin(), this->arg2_.begin(), this);
			it.adjust_forward();
			return it;
		}
		const_iterator end() const
		{
			return const_iterator(this->arg1_.end(), this->arg2_.end(), this);
		}

		//lower_bound

	public:
		template<class Sub>
            const_iterator lower_bound(const Sub& sub) const 
		{
			const bool shorter = 
				rel::size<typename Sub::field_list>::value <=
				rel::size<typename Table1::sort_list>::value;

			return lower_bound(sub, boost::mpl::bool_<shorter>());
		}

	private:
		template<class Sub>
            const_iterator lower_bound(const Sub& sub, short_subtuple) const 
		{
			const_iterator it(this->arg1_.lower_bound(sub), typename arg2_type::const_iterator(), this);
			it.adjust_forward();
			return it;
		}
		template<class Sub>
            const_iterator lower_bound(const Sub& sub, long_subtuple) const 
		{
			typedef split<Sub, Table1> split_type;

			const_iterator it(this->arg1_.lower_bound(split_type::tuple1(sub)), 
				typename arg2_type::const_iterator(), this);

			if (it.adjust_forward())
				return it;

			if (it == end())
				return it;

			it.itr2_ = range(it.begin_, it.end_).lower_bound(split_type::tuple2(sub)).base();

			if (it.itr2_ == it.end_)
			{
				++it.itr1_;
				it.adjust_forward();
			}

			return it;
		}

		//upper_bound

	public:
		template<class Sub>
            const_iterator upper_bound(const Sub& sub) const 
		{
			const bool shorter = 
				rel::size<typename Sub::field_list>::value <=
				rel::size<typename Table1::sort_list>::value;

			return upper_bound(sub, boost::mpl::bool_<shorter>());
		}

	private:
		template<class Sub>
            const_iterator upper_bound(const Sub& sub, short_subtuple) const 
		{
			const_iterator it(this->arg1_.upper_bound(sub), typename arg2_type::const_iterator(), this);
			it.adjust_forward();
			return it;
		}
		template<class Sub>
            const_iterator upper_bound(const Sub& sub, long_subtuple) const 
		{
			typedef split<Sub, Table1> split_type;

			const_iterator it(this->arg1_.lower_bound(split_type::tuple1(sub)), 
				typename arg2_type::const_iterator(), this);

			if (it.adjust_forward())
				return it;

			if (it == end())
				return it;

			it.itr2_ = range(it.begin_, it.end_).upper_bound(split_type::tuple2(sub)).base();

			if (it.itr2_ == it.end_)
			{
				++it.itr1_;
				it.adjust_forward();
			}

			return it;
		}

		//equal_range

	public:
		template<class Sub>
            range_t<const_iterator> equal_range(const Sub& sub) const 
		{
			const bool shorter = 
				rel::size<typename Sub::field_list>::value <=
				rel::size<typename Table1::sort_list>::value;

			return equal_range(sub, boost::mpl::bool_<shorter>());
		}

	private:
		template<class Sub>
            range_t<const_iterator> equal_range(const Sub& sub, short_subtuple) const 
		{
			range_t<typename arg1_type::const_iterator> r = this->arg1_.equal_range(sub);
			const_iterator it1(r.begin().base(), typename arg2_type::const_iterator(), this);
			const_iterator it2(r.end().base(), typename arg2_type::const_iterator(), this);
			it1.adjust_forward();
			it2.adjust_forward();
			return range(it1, it2);
		}
		template<class Sub>
            range_t<const_iterator> equal_range(const Sub& sub, long_subtuple) const 
		{
			typedef split<Sub, Table1> split_type;

			const_iterator it1(this->arg1_.lower_bound(split_type::tuple1(sub)), 
				typename arg2_type::const_iterator(), this);

			const_iterator it2(it1);

			if (it1.adjust_forward())
				return range(it1, it1);

			if (it1 == end())
				return range(it1, it1);

			it2.adjust_forward();

			range_t<typename range_t<typename arg2_type::const_iterator>::const_iterator> r =
				range(it1.begin_, it1.end_).equal_range(split_type::tuple2(sub));
				
			it1.itr2_ = r.begin().base().base();
			it2.itr2_ = r.end().base().base();

			if (it1.itr2_ == it1.end_)
			{
				++it1.itr1_;
				it1.adjust_forward();
			}
			if (it2.itr2_ == it2.end_)
			{
				++it2.itr1_;
				it2.adjust_forward();
			}

			return range(it1, it2);
		}
	private:
		Begin begin_;
		End end_;
    };

	template<class Table1, class Table2, class Begin, class End>
	struct range_join_t : boost::mpl::identity<rj_t<Table1, Table2, Begin, End> >
	{};

	template<class Table1, class Table2, class Begin, class End>
        typename range_join_t<Table1, Table2, Begin, End>::type
        range_join(const Table1& t1, const Table2& t2, const Begin& b, const End& e)
	{
		return typename range_join_t<Table1, Table2, Begin, End>::type(t1, t2, b, e);
	}

	// equal join

	template<class T1, class T2, class FieldList>
	struct equal_join_t : range_join_t<T1, T2, lower_bound_t<FieldList>, upper_bound_t<FieldList> >
	{};

	template<class FieldList, class T1, class T2>
		typename equal_join_t<T1, T2, FieldList>::type
		equal_join(const T1& t1, const T2& t2)
	{
		return range_join(t1, t2, lower_bound<FieldList>(), upper_bound<FieldList>());
	}
}}

#endif//BOOST_RTL_RANGE_JOIN_HPP_INCLUDED
