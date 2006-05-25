// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_LIST_HPP_INCLUDED
#define BOOST_RTL_LIST_HPP_INCLUDED

#include <boost/rtl/generic.hpp>
#include <boost/mpl/advance.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/bind.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/count_if.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/reverse_fold.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/reverse_iter_fold.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/iterator_tags.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/type_traits/is_same.hpp>

// some MSVC related workarounds

#if defined(BOOST_MPL_MSVC_ETI_BUG)
namespace boost
{
	namespace mpl
	{
		template<>
			struct push_front<int, int>
		{
			typedef int type;
		};
	}
}
#endif

namespace boost { namespace rel {

	// breakable_for_each

	template<class Begin, class End, class F>
		struct breakable_for_each_t	
	{
		typedef typename boost::mpl::bool_<boost::is_same<Begin, End>::value>::type done;
		
		static void run(F f)
		{
			do_run<int>(f, done());
		}
        template<class Unused>
            static void do_run(F f, boost::mpl::false_)
		{
            typename boost::mpl::deref<Begin>::type param;
			if (f(param))
                breakable_for_each_t<typename boost::mpl::next<Begin>::type, End, F>::run(f);
		}
        template<class Unused>
    		static void do_run(F f, boost::mpl::true_)
		{}
	};
	template<class Sequence, class F>
		void breakable_for_each(F f, Sequence* = 0)
	{
		breakable_for_each_t<
			typename boost::mpl::begin<Sequence>::type, 
			typename boost::mpl::end<Sequence>::type, 
			F>::run(f);
	}

	// list_concat

	template<class Seq1, class Seq2>
		struct concat 
            : boost::mpl::joint_view<Seq1, Seq2>
	{};

	// list_contains

	template<class Seq, class T>
		struct list_contains
	{
		typedef typename boost::mpl::contains<Seq, T>::type type;
		BOOST_STATIC_CONSTANT(bool, value = list_contains::type::value);
	};

	// list_contains_all

	template<class Seq, class Begin, class End>
		struct list_contains_all_helper;

	template<class Seq, class Begin, class End>
		struct list_contains_all_impl
	{
		typedef typename boost::mpl::eval_if<
			boost::mpl::bool_<boost::is_same<Begin, End>::value>,
			boost::mpl::identity<boost::mpl::true_>,
			list_contains_all_helper<Seq, Begin, End>
			>::type type;
	};

	template<class Seq, class Begin, class End>
		struct list_contains_all_helper
	{
		typedef typename boost::mpl::eval_if<
            typename boost::mpl::contains<Seq, typename boost::mpl::deref<Begin>::type>::type,
            list_contains_all_impl<Seq, typename boost::mpl::next<Begin>::type, End>,
			boost::mpl::identity<boost::mpl::false_>
			>::type type;
	};

	template<class Seq1, class Seq2>
		struct list_contains_all
	{
		typedef typename list_contains_all_impl<
			Seq1, 
			typename boost::mpl::begin<Seq2>::type, 
			typename boost::mpl::end<Seq2>::type
			>::type type;

		BOOST_STATIC_CONSTANT(bool, value = list_contains_all::type::value);
	};

	// list_generate_prefixes

	template<class Sequence>
		struct generate_prefixes
	{
		struct functor
		{
			template<class State, class It>
				struct apply
			{
				typedef typename boost::mpl::push_front<
					State, 
					boost::mpl::iterator_range<typename boost::mpl::begin<Sequence>::type, It>
					>::type type;
			};
		};
		typedef typename boost::mpl::reverse_iter_fold<
			Sequence, 
			boost::mpl::vector1<Sequence>, 
			functor>::type type;
	};

	// list_size

	template<class Seq> 
    struct size
	{
        typedef typename boost::mpl::distance<
            typename boost::mpl::begin<Seq>::type, 
            typename boost::mpl::end<Seq>::type
        >::type type;

        BOOST_STATIC_CONSTANT(int, value = type::value);
	};

	// list_index_of
	template<class Seq, class T>
		struct list_index_of
	{
        typedef typename boost::mpl::begin<Seq>::type first;
		typedef typename boost::mpl::find<Seq, T>::type last;

        typedef typename boost::mpl::if_c<
            boost::is_same<last, typename boost::mpl::end<Seq>::type>::value,
            boost::mpl::int_<-1>,
            typename boost::mpl::distance<first, last>::type
        >::type type;

        BOOST_STATIC_CONSTANT(int, value = type::value);
	};

	// list_prefix

	template<class Seq, int n>
		struct list_prefix
	{
		// range(begin, begin + n)

		typedef typename boost::mpl::if_c<
			(n < rel::size<Seq>::value),
			boost::mpl::int_<n>, 
			boost::mpl::int_<rel::size<Seq>::value>
			>::type prefix_size; 

		typedef boost::mpl::iterator_range<
			typename boost::mpl::begin<Seq>::type,
			typename boost::mpl::advance<
				typename boost::mpl::begin<Seq>::type,
				prefix_size
				>::type
			> type;
	};

	// list_suffix

	template<class Seq, int n>
		struct list_suffix
	{
		// range(begin + n, end)

		typedef boost::mpl::iterator_range<
			typename boost::mpl::advance<
				typename boost::mpl::begin<Seq>::type, 
				boost::mpl::int_<n> 
				>::type,
			typename boost::mpl::end<Seq>::type
			> type;
	};

	// list_difference

	template<class Seq1, class Seq2>
		struct list_difference : boost::mpl::reverse_fold<
			Seq1, 
			boost::mpl::vector0<>, 
			boost::mpl::if_<
				boost::mpl::contains<Seq2, boost::mpl::_2>,
				boost::mpl::_1,
				boost::mpl::push_front<boost::mpl::_1, boost::mpl::_2>
				>
			>
	{};

	// list_intersection

	template<class Seq1, class Seq2>
		struct list_intersection : boost::mpl::reverse_fold<
			Seq1, 
			boost::mpl::vector0<>, 
			boost::mpl::if_<
				boost::mpl::contains<Seq2, boost::mpl::_2>,
				boost::mpl::push_front<boost::mpl::_1, boost::mpl::_2>,
				boost::mpl::_1
				>
			>
	{};

	// list_union

	template<class Seq1, class Seq2>
		struct list_union : concat<
			typename list_difference<Seq1, Seq2>::type,
			Seq2
			>
	{};

	// create_pairs

	template<class Begin1, class Begin2, int n>
		struct create_pairs_impl;

	template<class Begin1, class Begin2, int n>
		struct create_pairs_helper : boost::mpl::push_front<
			typename create_pairs_impl<
				typename boost::mpl::next<Begin1>::type, 
				typename boost::mpl::next<Begin2>::type, 
				(n - 1)
				>::type,
                boost::mpl::pair<typename boost::mpl::deref<Begin1>::type, typename boost::mpl::deref<Begin2>::type>
			>
	{};
	template<class Begin1, class Begin2, int n>
		struct create_pairs_impl : boost::mpl::eval_if_c<
			(n == 0),
			boost::mpl::vector0<>,
			create_pairs_helper<Begin1, Begin2, n>
			>
	{};
	template<class Seq1, class Seq2, int n>
		struct create_pairs : create_pairs_impl<
			typename boost::mpl::begin<Seq1>::type, 
			typename boost::mpl::begin<Seq2>::type, 
			n
			>
	{};

	// find_value

	template<class Pairs, class Key> 
		struct find_value
	{
		struct predicate
		{
			template<class Pair>
				struct apply : boost::is_same<
					typename Pair::first,
					Key
				>
			{};
		};
		template<class Iter>
			struct extract_value : boost::mpl::identity<
				typename Iter::type::second
			>
		{};

		typedef typename boost::mpl::find_if<
			Pairs, 
			predicate
			>::type iter;

		typedef typename boost::mpl::eval_if_c<
			boost::is_same<iter, typename boost::mpl::end<Pairs>::type>::value,
			boost::mpl::identity<null_type>,
			extract_value<iter>
			>::type type;
	};

	//full_sublist_conversion

	template<class Pairs, class List>
		struct full_sublist_conversion 
	{
		struct functor
		{
			template<class State, class T>
				struct apply
			{
				typedef typename find_value<Pairs, T>::type value;
				typedef typename boost::mpl::eval_if_c<
					boost::is_same<value, null_type>::value,
					boost::mpl::identity<State>,
					boost::mpl::push_front<State, value>
					>::type type;
			};
		};
		typedef typename boost::mpl::reverse_fold<
			List,
			boost::mpl::vector0<>,
			functor
			>::type type;
	};

	// short_sublist_conversion

	template<class Pairs, class Begin, class End>
		struct short_sublist_conversion_impl;

	template<class Pairs, class Begin, class End>
		struct short_sublist_conversion_helper
	{
        typedef typename find_value<Pairs, typename boost::mpl::deref<Begin>::type>::type value;

		typedef typename boost::mpl::eval_if_c<
			boost::is_same<value, null_type>::value,
			boost::mpl::identity<boost::mpl::vector0<> >,
			boost::mpl::push_front<
				typename short_sublist_conversion_impl<Pairs, typename boost::mpl::next<Begin>::type, End>::type,
				value>
			>::type type;
	};
	template<class Pairs, class Begin, class End>
		struct short_sublist_conversion_impl : boost::mpl::eval_if_c<
			boost::is_same<Begin, End>::value,
			boost::mpl::vector0<>,
			short_sublist_conversion_helper<Pairs, Begin, End>
			>
	{};
	template<class Pairs, class List>
		struct short_sublist_conversion : short_sublist_conversion_impl<
			Pairs, 
			typename boost::mpl::begin<List>::type, 
			typename boost::mpl::end<List>::type>
	{};

	// make_reverse

	struct make_reverse_functor
	{
		template<class State, class Pair>
			struct apply : boost::mpl::push_front<
				State,
				boost::mpl::pair<
					typename Pair::second,
					typename Pair::first
				>
			>
		{};
	};
	template <class Pairs>
		struct make_reverse : boost::mpl::reverse_fold<
			Pairs,
			boost::mpl::vector0<>,
			make_reverse_functor
		>
	{};

	// filter_pairs (has intersection semantics)

	template<class List>
		struct filter_pairs_functor
	{
		template<class State, class Pair>
			struct apply : boost::mpl::eval_if<
				typename boost::mpl::contains<List, typename Pair::first>::type,
				boost::mpl::push_front<State, Pair>,
				boost::mpl::identity<State>
			>
		{};
	};
	template<class Pairs, class List>
		struct filter_pairs : boost::mpl::reverse_fold<
			Pairs, 
			boost::mpl::vector0<>,
			filter_pairs_functor<List>
		>
	{};

	// rename_pairs

	template<class T, class A> struct alias;

	template<class A>
		struct rename_pairs_functor
	{
		template<class T>
			struct apply
		{
            typedef typename boost::mpl::pair<T, alias<T, A> > type;
        };
	};
	template<class Seq, class A = null_type>
		struct rename_pairs : boost::mpl::transform_view<
			Seq,
			rename_pairs_functor<A>
		>
	{};

	// list_equal

	template<class Seq1, class Seq2>
		struct list_equal
	{
		typedef typename boost::mpl::equal<Seq1, Seq2>::type type;
		BOOST_STATIC_CONSTANT(bool, value = list_equal::type::value);
	};
}}

#endif//BOOST_RTL_LIST_HPP_INCLUDED
