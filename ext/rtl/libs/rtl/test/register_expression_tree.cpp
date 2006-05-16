#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/tuple.hpp>
#include <iostream>
#include <boost/rtl/select_functors.hpp>
#include <boost/rtl/utils.hpp>
#include <boost/rtl/table_delta.hpp>
#include <boost/rtl/crossproduct_delta.hpp>
#include <boost/rtl/key_index_delta.hpp>
#include <boost/rtl/iterator_index_delta.hpp>
#include <boost/rtl/selection_delta.hpp>
#include <boost/rtl/traverse.hpp>
#include <boost/rtl/cover_delta.hpp>
#include "table_defs.hpp"
#include <boost/rtl/key_delta.hpp>

using namespace boost;
using namespace rel;
using namespace std;
using namespace lambda;

//
// This file was modified to eliminate linker's duplicate comdat error.
// mpl vectors and tables were wrapped by structures.
// 

namespace rx
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	BOOST_RTL_DEFINE_COLUMN(int, c2);

	struct f1 : mpl::vector1<c1> {};
	struct f2 : mpl::vector1<c2> {};
	struct f12 : mpl::vector2<c1, c2> {};

	BOOST_RTL_TABLE_1(tab1, f1);
	BOOST_RTL_TABLE_1(tab2, f2);	

	typedef tab1::value_type tuple_1; // added
	typedef tab2::value_type tuple_2; // added

	tab1 t1;
	tab2 t2;

	typedef key_index_t<tab2,f2>::type ki2;
	typedef key_index_t<tab1,f1>::type ki1;
	typedef cover_t<ki1::value_type,ki1::sort_list,ki1::keys> cv;
	typedef cross_product_t<cv,ki2>::type cpi;
	typedef iterator_index_t<cpi,f12>::type ii;
	typedef selection_t<ii,le<c1> >::type sc;

	template<class R> 
		void bar(R r)
//	void bar(sc r)
	{
		expression_registry exprs;
		exprs.add(r);

		transaction tr;
		
		for (int i = 0; i < 10; ++i)
		{
			tr.insert(t1, tuple_1(i)); 
			tr.insert(t2, tuple_2(i * 10));
		}

		cout << "modified" << endl;
		print(modified(r, tr));

		tr.commit(exprs);
		
		cout << "after commit" << endl;
		print(r);
	}

	void foo_1()
	{
		bar(selection(key_index<f2>(t2), lambda::_1[c2()] <= 3));
	}

	void foo_2()
	{
		bar(key<boost::mpl::vector1<boost::mpl::vector1<c1> > >(iterator_index<f1>(t1)));
//		bar(iterator_index<f1>(t1));
	}
	


	void foo()
	{
/*
		sc SC = selection
			(
				iterator_index<f12>(
					cross_product(

						cover(
						key_index<f1>(t1)
						)
						, 
						key_index<f2>(t2)))
					,
				le<c1>(5));



		bar(
//			selection(
//				iterator_index<f12>(
//					cross_product(
//
//						cover(
//						key_index<f1>(t1)
//						)
//						, 
//						key_index<f2>(t2)))
//					,
//				le<c1>(5))
			SC
				);
*/
	}


	unit::test_function _a("register_expression_1", foo_1);
	unit::test_function _b("register_expression_2", foo_2);
}
