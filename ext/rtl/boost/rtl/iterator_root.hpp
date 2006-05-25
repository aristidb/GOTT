// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_ITERATOR_ROOT_HPP_INCLUDED
#define BOOST_RTL_ITERATOR_ROOT_HPP_INCLUDED

#define BOOST_RTL_PULL_ITERATOR_TYPEDEFS(Base)\
	typedef typename Base::iterator_category iterator_category;\
	typedef typename Base::value_type value_type;\
	typedef typename Base::difference_type difference_type;\
	typedef typename Base::reference reference;\
	typedef typename Base::relation_type relation_type;

namespace boost { namespace rel {

	template <class Derived, class Relation> 
		class iterator_root
	{
	public:
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef typename Relation::value_type value_type;
		typedef ptrdiff_t difference_type;
		typedef value_type reference;
		typedef Relation relation_type;

	public:
		const relation_type* relation() const
		{
			return relation_;
		}

		bool operator!= (const Derived& other) const
		{
			return !(static_cast<const Derived&>(*this) == other);
		}

		Derived& operator++() 
		{
			static_cast<Derived&>(*this).increment();
			return static_cast<Derived&>(*this);
		}

		Derived operator++(int) 
		{
			Derived tmp(static_cast<Derived&>(*this));
			static_cast<Derived&>(*this).increment();
			return tmp;
		}

		Derived& operator--() 
		{
			static_cast<Derived&>(*this).decrement();
			return static_cast<Derived&>(*this);
		}

		Derived operator--(int) 
		{
			Derived tmp(static_cast<Derived&>(*this));
			static_cast<Derived&>(*this).decrement();
			return static_cast<Derived&>(*this);
		}

	protected:
		typedef iterator_root base_type;
		iterator_root(const Relation* o = 0) 
			: relation_(o) 
		{}

		// This is get support
	public:
		template<class Column>
			typename Column::type
		get(const Column& c) const 
		{
			const Derived& itr = static_cast<const Derived&>(*this);
			return itr.get_internal(c);
		}

        template<class Column>
			typename Column::type
		operator[](const Column& c) const 
		{
			const Derived& itr = static_cast<const Derived&>(*this);
			return itr.get_internal(c);
		}
	public:
		template<class Tuple>
			class set_field_generic
		{
		public:
			set_field_generic(const Derived& i, Tuple& t) : i_(i), t_(t) {}
			template<class Column> void operator()(Column& c)
			{
				t_[c] = i_.get(c);
			}
		private:
			const Derived& i_;
			Tuple& t_;
		};

	public:
		value_type operator*() const
		{
			value_type tup;
			set_field_generic<value_type> s(static_cast<const Derived&>(*this),tup);
			boost::mpl::for_each<typename value_type::field_list>(s);
			return tup;
		}

	protected:
		const relation_type* relation_;
	};

	namespace detail
	{
		// cmp_it auxilary class for comparing iterators
		template <class Iter1, class Iter2> 
			class cmp_it 
		{
		public:
			cmp_it(const Iter1& i1, const Iter2& i2, int& result) 
				: i1_(i1) 
				, i2_(i2)
				, result_(result) 
			{
			}
			template<class T> 
				bool operator()(const T& c)
			{
				bool return_value = true;
				typename T::type r1 = i1_.get(c);
				typename T::type r2 = i2_.get(c);
				bool temp = std::less<typename T::type>()(r1,r2);
				if (temp || std::less<typename T::type>()(r2,r1))
				{
					result_ = temp ? -1 : 1;
					return_value = false;
				}
				return return_value;
			}

			template<class T1, class T2>
				bool operator()(const T1& c1, const T2& c2)
			{
				bool return_value = true;
				typename T1::type r1 = i1_.get(c1);
				typename T2::type r2 = i2_.get(c2);
				bool temp = std::less<typename T1::type>()(r1,r2);
				if (temp || std::less<typename T1::type>()(r2,r1))
				{
					result_ = temp ? -1 : 1;
					return_value = false;
				}
				return return_value;
			}

		private:
			const Iter1& i1_;
			const Iter2& i2_;
			int& result_;
		};

		template <class Iter1, class Iter2> 
			class cmp_diff_it 
		{
		public:
			cmp_diff_it(const Iter1& i1, const Iter2& i2, int& result) 
				: i1_(i1) 
				, i2_(i2)
				, result_(result) 
			{
			}

			template<class P>
				bool operator()(const P&)
			{
				bool return_value = true;
				typedef typename P::first c1;
				typedef typename P::second c2;
				typename c1::type r1 = i1_.get(c1());
				typename c2::type r2 = i2_.get(c2());
				bool temp = std::less<typename c1::type>()(r1,r2);
				if (temp || std::less<typename c1::type>()(r2,r1))
				{
					result_ = temp ? -1 : 1;
					return_value = false;
				}
				return return_value;
			}

		private:
			const Iter1& i1_;
			const Iter2& i2_;
			int& result_;
		};


	}

	// compare_iterators
	template<class FieldList, class Iter1, class Iter2>
		int compare_iterators(const Iter1& i1, const Iter2& i2, const FieldList* = 0)
	{
		int result = 0;  // initialize for empty list comparisons
		breakable_for_each<FieldList>(detail::cmp_it<Iter1, Iter2>(i1, i2, result));
		return result;
	}

	template<class PairList, class Iter1, class Iter2>
		int compare_different_iterators(const Iter1& i1, const Iter2& i2, const PairList* = 0)
	{
		int result = 0;  // initialize for empty list comparisons
		breakable_for_each<PairList>(detail::cmp_diff_it<Iter1, Iter2>(i1, i2, result));
		return result;
	}

}}

#endif//BOOST_RTL_ITERATOR_ROOT_HPP_INCLUDED
