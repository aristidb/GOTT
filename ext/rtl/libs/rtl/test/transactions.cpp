#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include <iostream>
#include <set>
#include <vector>
#include <utility>
#include <boost/smart_ptr.hpp>
#include <boost/rtl/utils.hpp>
#include "test_function.hpp"
#include <boost/rtl/table_delta.hpp> 
#include <boost/rtl/key_index_delta.hpp>
#include <boost/rtl/iterator_index_delta.hpp>
#include <boost/rtl/rename_delta.hpp>
#include <boost/rtl/crossproduct_delta.hpp>
#include <boost/rtl/selection_delta.hpp>
#include <boost/rtl/projection_delta.hpp>
#include <boost/rtl/cover_delta.hpp>
#include <boost/rtl/merge_delta.hpp>
#include <boost/rtl/zigzag_delta.hpp>
#include <boost/rtl/range_selection_delta.hpp>
#include <boost/rtl/groupby_delta.hpp>
#include <boost/rtl/expression_registry.hpp>

using namespace unit;
using namespace boost;
using namespace rel;
using namespace std;


namespace transactions_cpp
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	BOOST_RTL_DEFINE_COLUMN(int, c2);

	typedef table<table_info<mpl::vector1<c1> > > table_t;
	typedef table_t::value_type tuple_t; 

	typedef table<table_info<mpl::vector2<c1, c2> > > table2_t;
	typedef table2_t::value_type tuple2_t; 

	void foo()
	{
		table_t t1, t2(t1);
		TESTASSERT(t1.id() == t2.id());

		transaction tr;
		tr.insert(t1, tuple_t(1));
		tr.insert(t1, tuple_t(2));
		tr.insert(t2, tuple_t(3));

		table2_t t3;
		tr.insert(t3, tuple2_t(1, 3));
		tr.insert(t3, tuple2_t(0, 4));
		tr.insert(t3, tuple2_t(6, 4));

		cout << "before commit, t1:" << endl; 
		print(t1);
		cout << "delta:" << endl;
		print(delta(t1, tr).first);
		cout << "t3:" << endl; 
		print(t3);
		cout << "delta:" << endl;
		print(delta(t3, tr).first);

		tr.commit();

		cout << "after commit, t1:" << endl; 
		print(t1);
		cout << "delta:" << endl;
		print(delta(t1, tr).first);
		cout << "t3:" << endl; 
		print(t3);
		cout << "delta:" << endl;
		print(delta(t3, tr).first);

	}

	test_function test("transactions", foo);
}

namespace dbtr_cpp
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	BOOST_RTL_DEFINE_COLUMN(int, c2);

	typedef table<table_info<mpl::vector2<c1, c2> > > table_t;
	typedef table_t::value_type tuple_t;

	void foo()
	{
		table_t t;
		int i;
		
		for (i = 0; i < 5; ++i)
			t.insert(tuple_t(i, 9 - i));

		typedef key_index_t<table_t, mpl::vector2<c2, c1> >::type index_t;

		index_t idx(t);

		expression_registry exprs;
		exprs.add(idx);

		transaction tr;

		for (i = 5; i < 10; ++i)
			tr.insert(t, tuple_t(i, 9 - i));

		print(t);
		print(modified(t, tr));
		print(idx);
		print(modified(idx, tr));

		TESTASSERT(equal_tables(modified(idx, tr), key_index<mpl::vector1<c2> >(modified(t, tr))));

		tr.commit(exprs);

		print(t);
		print(modified(t, tr));
		print(idx);
		print(modified(idx, tr));

		TESTASSERT(equal_tables(idx, index_t(t)));

		// deltas should be empty now
		TESTASSERT(equal_tables(delta(t, tr).first, table_t()));
		TESTASSERT(equal_tables(delta(idx, tr).first, index_t()));

		// modified should equal to regular
		TESTASSERT(equal_tables(t, modified(t, tr)));
		TESTASSERT(equal_tables(idx, modified(idx, tr)));
	}

	test_function test("database_transactions", foo);
}

namespace iterator_index_test
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	typedef table<table_info<mpl::vector1<c1>, mpl::vector0<> > > table_t;

	void foo()
	{
		table_t t;
		iterator_index_t<table_t, mpl::vector1<c1> >::type idx(t);
		expression_registry exprs;
		exprs.add(idx);
		transaction tr;
		tr.insert(t, row<mpl::vector1<c1> >(5));
		tr.insert(t, row<mpl::vector1<c1> >(1));
		tr.insert(t, row<mpl::vector1<c1> >(3));
		tr.insert(t, row<mpl::vector1<c1> >(2));
		tr.insert(t, row<mpl::vector1<c1> >(4));
		print(modified(idx, tr));
		tr.commit(exprs);
		print(idx);
	}
	test_function test("iterator_index_test", foo);
}

