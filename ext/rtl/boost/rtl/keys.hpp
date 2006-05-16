// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_KEYS_HPP_INCLUDED
#define BOOST_RTL_KEYS_HPP_INCLUDED

#include <boost/rtl/list.hpp>
#include <boost/mpl/remove_if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/pop_back.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/insert_range.hpp>

namespace boost { namespace rel {

	// A key of a relation is a set of the relation's attributes
	// which fully defines a tuple in the relation. 
	// Each relation has set of keys. We say key k1 is stronger, than
	// k2, if k1 is subset of k2.
	// While adding new key to an existing set of keys we 
	// 1) Check if there is an existing key, stronger than one we attempt to add.
	// If it is true, we do not add the new key to collection, otherwise 
	// 2) we remove all keys, that are weaker and add the new key.


	template<class NewKey>
		struct key_is_stronger_f
	{
		template<class OldKey>
			struct apply
		{
			typedef typename list_contains_all<OldKey,NewKey>::type type;
		};
	};

	template<class NewKey>
		struct key_is_weaker_f
	{
		template<class OldKey>
			struct apply
		{
			typedef typename list_contains_all<NewKey,OldKey>::type type;
		};
	};

	template<class NewKey, class Keys>
		struct remove_weak_keys
	{
		typedef typename boost::mpl::remove_if<Keys, key_is_stronger_f<NewKey> >::type type;
	};


	template<class NewKey, class Keys>
		struct key_is_weaker
	{
		typedef typename boost::mpl::find_if<Keys, key_is_weaker_f<NewKey> >::type itr;
		typedef typename boost::mpl::end<Keys>::type e;
		enum {val = !boost::is_same<itr,e>::value};
		BOOST_STATIC_CONSTANT(bool, value = val); 
	};


	template<class NewKey, class Keys>
		struct key_add_helper
	{
		typedef typename boost::mpl::push_front
		<
			typename remove_weak_keys<NewKey,Keys>::type,
			NewKey
		>::type type;
	};
		
	template<class NewKey, class Keys>
		struct key_add
	{
		typedef typename boost::mpl::eval_if
		<
			key_is_weaker<NewKey,Keys>,
			boost::mpl::identity<Keys>,
			key_add_helper<NewKey,Keys>
		>::type type;
	};

	struct key_add_f
	{
		template<class Keys, class NewKey>
			struct apply
		{
			typedef typename key_add<NewKey,Keys>::type type;
		};
	};

	template<class OldKeys, class NewKeys>
		struct keys_concat
	{
		typedef typename boost::mpl::fold
		<
			NewKeys,
			OldKeys,
			key_add_f
		>::type type;
	};

	// find prefix of SortList, that contains one of Keys

	template<class SortList, class Keys>
		struct optimize_helper
	{
		template<class PrefixSize>
			struct apply
		{
			typedef typename list_prefix<SortList,PrefixSize::value>::type prefix;
			typedef typename boost::mpl::bool_<key_is_weaker<prefix,Keys>::value>::type type;
		};
	};

	template<class SortList, class Keys>
		struct optimize_sort
	{
		typedef typename boost::mpl::find_if
		<
			boost::mpl::range_c<int,0,rel::size<SortList>::value>,
			optimize_helper<SortList,Keys> 
		>::type v;
		typedef typename list_prefix<SortList,v::type::value>::type type;
	};


	// crossproduct_keys

	template<class Key1>
		struct crossproduct_helper_2
	{
		template<class State,class Key2>
			struct apply
		{
			typedef typename concat<Key1,Key2>::type crossproduct_key;    // from list
			typedef typename key_add<crossproduct_key,State>::type type;
		};
	};

	template<class Key1,class Keys2,class State>
		struct crossproduct_internal
	{
//		typedef typename boost::mpl::reverse_fold
		typedef typename boost::mpl::fold
		<
			Keys2,
			State,
			crossproduct_helper_2<Key1>
		>::type type;
	};

	template<class Keys2>
		struct crossproduct_helper_1
	{
		template<class State,class Key1>
			struct apply
		{
			typedef typename crossproduct_internal<Key1,Keys2,State>::type type;
		};
	};

	template<class Keys1, class Keys2>
		struct crossproduct_keys
	{
		typedef typename boost::mpl::reverse_fold
		<
			Keys1,
			boost::mpl::vector0<>,
			crossproduct_helper_1<Keys2>
		>::type type;
	};

	// difference/diff - assume keys of 1st operand.

	template<class Keys>
		struct difference_keys
	{
		typedef Keys type;
	};

	// groupby_keys - TODO!!! first solve value_type issue. What if GroupList and 
	// KeyList (bad name!) do not produce field list?
	// Proposal - name GroupList only (FoldList sounds better). Assume all other 
	// fields belong to the the the key list

	template<class KeyList>
		struct groupby_helper
	{
		template<class State, class Key>
			struct apply
		{
			typedef typename boost::mpl::eval_if
			<
				list_contains_all<KeyList,Key>,
				key_add<Key,State>,
				boost::mpl::identity<State>
			>::type type;
		};
	};

	template<class Keys, class KeyList>
		struct groupby_keys
	{
		typedef typename boost::mpl::reverse_fold
		<
			Keys,
			boost::mpl::vector1<KeyList>,
			groupby_helper<KeyList>
		>::type type;
	};

	// index/iterator_index/key_index do not affect keys

	// intersection//isection
	// add keys from 2nd relation to keys of 1st relation 

	template<class Keys1, class Keys2>
		struct intersection_keys
	{
		typedef typename boost::mpl::reverse_fold
		<
			Keys2,
			Keys1,
			key_add_f
		>::type type;
	};

	// join uses crossproduct keys

	// merge has the same keys, as crossroduct, but after 
	// keys are concatenated, first n fields from 2nd relation 
	// sort list (duplicates) are filtered out.

	template<class Key1, class Duplicates>
		struct merge_helper_2
	{
		template<class State, class Key2>
			struct apply
		{
			typedef typename concat<Key1,typename list_difference<Key2,Duplicates>::type>::type merge_key;
			typedef typename key_add<merge_key,State>::type type;
		};
	};

	template<class Key1, class Keys2, class State, class Duplicates>
		struct merge_internal
	{
		typedef typename boost::mpl::fold
		<
			Keys2,
			State,
			merge_helper_2<Key1,Duplicates>
		>::type type;
	};

	template<class Keys2, class Duplicates>
		struct merge_helper_1
	{
		template<class State, class Key1>
			struct apply
		{
			typedef typename merge_internal<Key1,Keys2,State,Duplicates>::type type;
		};
	};

	template<class Keys1, class Keys2, class Duplicates>
		struct merge_keys
	{
		typedef typename boost::mpl::reverse_fold
		<
			Keys1,
			boost::mpl::vector0<>,
			merge_helper_1<Keys2,Duplicates>
		>::type type;
	};

	// Projection 
	
	template<class FieldList>
		struct projection_helper
	{
		template<class State, class Key>
			struct apply
		{
			typedef typename boost::mpl::eval_if
			<
				typename list_contains_all<FieldList,Key>::type,
				boost::mpl::push_front<State,Key>,
				boost::mpl::identity<State>
			>::type type;
		};
	};

	template<class Keys, class FieldList>
		struct projection_keys
	{
		typedef typename boost::mpl::reverse_fold
		<
			Keys,
			boost::mpl::vector0<>,
			projection_helper<FieldList>
		>::type type;
	};


	// Find longest prefix of sort list, which is contained in  
	// given field list


	template<class SortList, class FieldList>
		struct dprj_sort_helper
	{
		template<class PrefixSize>
			struct apply
		{
			enum {psize = rel::size<SortList>::type::value-PrefixSize::type::value};
			typedef typename list_prefix<SortList,psize>::type prefix;
			typedef typename list_contains_all<FieldList,prefix>::type type;
		};
	};

	struct cp_pred
	{
		template<class State, class Item>
			struct apply
		{
			typedef typename boost::mpl::push_back<State,Item>::type type;
		};
	};

	template<class Sequence>
		struct cp
	{
		typedef typename boost::mpl::fold
		<
			Sequence,
			boost::mpl::vector0<>,
			cp_pred
		>::type type;
	};


	template<class SortList, class FieldList>
		struct dprj_sort
	{
		typedef typename boost::mpl::find_if
		<
			boost::mpl::range_c<int,0,rel::size<SortList>::value>,
			dprj_sort_helper<SortList,FieldList> 
		>::type v;
		typedef typename list_prefix<SortList,v::type::value>::type xxx;
		typedef typename boost::mpl::insert_range<boost::mpl::vector0<>,boost::mpl::begin<boost::mpl::vector0<> >::type, xxx>::type type;
	};


	// Rename

	template<class Pairs>
		struct rename_helper
	{
		template<class State, class Key>
			struct apply
		{
			typedef typename boost::mpl::push_front
			<
				State,
				typename full_sublist_conversion<Pairs,Key>::type
			>::type type;
		};
	};

	template<class Keys, class Pairs>
		struct rename_keys
	{
		typedef typename boost::mpl::reverse_fold
		<
			Keys,
			boost::mpl::vector0<>,
			rename_helper<Pairs>
		>::type type;
	};

	// selection uses same keys

	// union. In case of union even if a key is present in both arguments it
	// can become no key in the union. Only keys from 1st sort-list that belong to common
	// for both relations sort-list survive.

	template<class FieldList>
		struct union_helper
	{
		template<class State, class Key>
			struct apply
		{
			typedef typename boost::mpl::eval_if
			<
				typename list_contains_all<FieldList,Key>::type,
				boost::mpl::push_front<State,Key>,
				boost::mpl::identity<State>				
			>::type type;
		};
	};

	template<class Keys1, class Keys2, class FieldList>
		struct union_keys
	{
		typedef typename boost::mpl::reverse_fold
		<
			Keys1,
			boost::mpl::vector0<>,
			union_helper<FieldList>
		>::type type;
	};

}}

#endif

