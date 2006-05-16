// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_TUPLE_HPP_INCLUDED
#define BOOST_RTL_TUPLE_HPP_INCLUDED

#include <boost/rtl/list.hpp>
#include <boost/rtl/column.hpp>
#include <boost/mpl/reverse_fold.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/preprocessor.hpp>

#define BOOST_RTL_FIELD(Tuple, Column) (Tuple).get(Column())

namespace boost { namespace rel {

	// derive_from

	template <class T, class U> 
		struct derive_from : public T, public U
	{
		#define DEFINE_CTOR(z, n, name)									\
			template<BOOST_PP_ENUM_PARAMS(n, class T)>					\
				name(BOOST_PP_ENUM_BINARY_PARAMS(n, const T, &v))		\
				: T(v0), U(BOOST_PP_ENUM_SHIFTED_PARAMS(n, v))			\
				{}

		BOOST_PP_REPEAT_FROM_TO(
			1,
			BOOST_PP_ADD(BOOST_MPL_LIMIT_VECTOR_SIZE, 1),
			DEFINE_CTOR,
			derive_from)

		#undef DEFINE_CTOR

		derive_from() 
		{}

		derive_from(const derive_from& a)
			: T(static_cast<const T&>(a))
			, U(static_cast<const U&>(a)) 
		{}
	};

	// tuple_base used to mark deleted tuples in some implementations

	struct tuple_base 
	{
		tuple_base() : deleted_(false){}	
		bool isDeleted() const {return deleted_;}
		void setDeleted(bool d) const {deleted_ = d;} 
		mutable bool deleted_;
	};
	struct empty
	{
		empty(){}
		empty(const empty&){} 
		empty& operator=(const empty&){return *this;} 
	}; 

	// derive_from_all

	template <class List> 
		struct derive_from_all
	{
		struct functor
		{
			template<class State, class Current>
				struct apply
			{
				typedef derive_from<typename Current::impl_type, State> type;
			};
		};
		typedef typename boost::mpl::reverse_fold<List, empty, functor>::type type;
	};

	// compare_fields

	template <class Tuple1, class Tuple2> 
		class compare_fields
	{
	public:
		compare_fields(const Tuple1& t1, const Tuple2& t2, int& result) 
			: t1_(t1), t2_(t2), result_(result) 
		{
			result_ = 0;
		}
		template <class T> 
			bool operator()(const T&)
		{
			bool temp;
			if (!(temp = std::less<typename T::type>()(BOOST_RTL_FIELD(t1_, T), BOOST_RTL_FIELD(t2_, T))) && 
				!std::less<typename T::type>()(BOOST_RTL_FIELD(t2_, T), BOOST_RTL_FIELD(t1_, T))) 
				return true;
			result_ = temp ? -1 : 1;
			return false;
		}
	private:
		const Tuple1& t1_;
		const Tuple2& t2_;
		int& result_;
	};

	// compare_tuples

	template <class SortList, class Tuple1, class Tuple2>
		int compare_tuples(const Tuple1& t1, const Tuple2& t2, SortList* = 0)
	{
		int result;
		breakable_for_each<SortList>(compare_fields<Tuple1, Tuple2>(t1, t2, result));
		return result;
	}

	// compare_different_fields DA
	template<class Tuple1, class Tuple2>
		class compare_different_fields
	{
	public:
		compare_different_fields(const Tuple1& t1, const Tuple2& t2, int& result) :
			t1_(t1), t2_(t2), result_(result)
		{
			result = 0;
		}

		template <class Pair> 
			bool operator()(const Pair&)
		{
			typedef typename Pair::first::type type;
			bool temp;
			if (!(temp =std::less<type>()(BOOST_RTL_FIELD(t1_, typename Pair::first), BOOST_RTL_FIELD(t2_, typename Pair::second))) && 
				!std::less<type>()(BOOST_RTL_FIELD(t2_, typename Pair::second), BOOST_RTL_FIELD(t1_, typename Pair::first))) 
				return true;
			result_ = temp ? -1 : 1;
			return false;
		}			
	private:
		const Tuple1& t1_;
		const Tuple2& t2_;
		int& result_;
	};

	// compare_different_tuples DA
	template<
		class SortList1, class SortList2, 
		int Len, class Tuple1, class Tuple2>
	int compare_different_tuples(const Tuple1& t1, const Tuple2& t2, 
		SortList1* = 0, SortList2* = 0, boost::mpl::int_<Len>* = 0)
	{
		int result;
		breakable_for_each<typename create_pairs<SortList1, SortList2, Len>::type>
			(compare_different_fields<Tuple1,Tuple2>(t1, t2, result));
		return result;
	}

	// copy_field

	template <class SourceTuple, class TargetTuple>
		class copy_field
	{
	public:
		copy_field(const SourceTuple& source_tuple, TargetTuple& target_tuple) 
			: source_tuple_(source_tuple), target_tuple_(target_tuple) {}

		template <class Column> 
			void operator()(const Column&)
		{
			target_tuple_.get(Column()) = source_tuple_.get(Column());
		}

	private:
		const SourceTuple& source_tuple_;
		TargetTuple& target_tuple_;
	};

	// copy fields

	template <class FieldList, class SourceTuple, class TargetTuple>
		void copy_fields(const SourceTuple& source_tuple, TargetTuple& target_tuple, FieldList* = 0)
	{
		boost::mpl::for_each<FieldList>(
			copy_field<SourceTuple, TargetTuple>(source_tuple, target_tuple));
	}

	// copy_different_fields, DA
	template<class SourceTuple, class TargetTuple>
	class copy_different_fields
	{
	public:
		copy_different_fields(const SourceTuple& source_tuple, TargetTuple& target_tuple) 
			: source_tuple_(source_tuple), target_tuple_(target_tuple) {}
		template <class Pair> 
			void operator()(const Pair&)
		{
			BOOST_RTL_FIELD(target_tuple_, typename Pair::second) = BOOST_RTL_FIELD(source_tuple_, typename Pair::first);
		}
	private:
		const SourceTuple& source_tuple_;
		TargetTuple& target_tuple_;
	};

	// copy_different_tuples, DA
	template<
		class SortList1, class SortList2, 
		int Len, class Tuple1, class Tuple2>
	void copy_different_tuples(const Tuple1& t1, Tuple2& t2, 
		SortList1* = 0, SortList2* = 0, boost::mpl::int_<Len>* = 0)
	{
		boost::mpl::for_each<
			typename create_pairs<SortList1, SortList2, Len>::type>(copy_different_fields<Tuple1,Tuple2>(t1, t2));
	}

	// row

	template <class FieldList> 
		class row : public tuple_base
	{
	public:
		typedef FieldList field_list;

	public:
		#define DEFINE_CTOR(z, n, name)									\
			template<BOOST_PP_ENUM_PARAMS(n, class T)>					\
				name(BOOST_PP_ENUM_BINARY_PARAMS(n, const T, &v))		\
				: fields_(BOOST_PP_ENUM_PARAMS(n, v))					\
				{}

		BOOST_PP_REPEAT_FROM_TO(
			1,
			BOOST_PP_ADD(BOOST_MPL_LIMIT_VECTOR_SIZE, 1),
			DEFINE_CTOR,
			row)

		#undef DEFINE_CTOR

		row() 
		{}

		template<class FL>
            row(const row<FL>& rhs)
        {
            BOOST_STATIC_ASSERT((list_equal<field_list, FL>::value));
            copy_fields<field_list>(rhs, *this);
        }
  
        row(const row& a)
			: tuple_base(static_cast<const tuple_base&>(a))
			, fields_(a.fields_)
		{}

		template <class Column>
			typename Column::reference get(const Column&)
			{
				return static_cast<typename Column::impl_type&>(fields_).value();
			}

		template <class Column>
			typename Column::const_reference get(const Column&) const
			{
				return static_cast<const typename Column::impl_type&>(fields_).value();
			}

        template <class Column>
			typename Column::reference operator[](const Column& c)
			{
				return get(c);
			}

		template <class Column>
			typename Column::const_reference operator[](const Column& c) const
			{
				return get(c);
			}

		bool operator== (const row& other) const
		{
			return compare_tuples<field_list>(*this, other) == 0;
		}

		bool operator!= (const row& other) const
		{
			return !(*this == other);
		}

	public:
		typename derive_from_all<FieldList>::type fields_;
	};

	// equal_range_cmp

	template<class FieldList>
		struct equal_range_cmp
	{
		template<class T1, class T2>
			bool operator()(const T1& t1, const T2& t2)
		{
			return compare_tuples<FieldList>(t1, t2) < 0;
		}
	};

	// valid_subtuple

	template <class SubTuple, class SortList>
		struct valid_subtuple : list_contains_all<
			typename list_prefix<SortList, rel::size<typename SubTuple::field_list>::value>::type,
			typename SubTuple::field_list
		>
	{};
}}

#endif//BOOST_RTL_TUPLE_HPP_INCLUDED
