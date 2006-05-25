#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/tuple.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/union.hpp>
#include "test_tables.hpp"
#include <boost/rtl/utils.hpp>
#include "test_decrement.hpp"
#include <boost/rtl/range.hpp>
#include "test_zigzag.hpp"
#include <boost/rtl/selection.hpp>
#include <boost/rtl/select_functors.hpp>
#include <boost/mpl/list.hpp>

using namespace boost;
using namespace rel;
using namespace unit;
using namespace lambda;

namespace union_cpp
{
	BOOST_RTL_DEFINE_COLUMN(int, num);
	BOOST_RTL_DEFINE_COLUMN(int, sec);

	void foo()
	{
		typedef table<table_info<mpl::vector1<num> > > table_t;
		typedef table_t::value_type tuple_t;

		union_t<table_t,table_t>::type::const_iterator i; // default constructible

		table_t t1, t2, result;

		t1.insert(tuple_t(2));
		t1.insert(tuple_t(6));
		t1.insert(tuple_t(8));
		t1.insert(tuple_t(3));
		t1.insert(tuple_t(4));
		t1.insert(tuple_t(9));

		t2.insert(tuple_t(6));
		t2.insert(tuple_t(8));
		t2.insert(tuple_t(5));
		t2.insert(tuple_t(7));
		t2.insert(tuple_t(1));

		result.insert(tuple_t(1));
		result.insert(tuple_t(2));
		result.insert(tuple_t(3));
		result.insert(tuple_t(4));
		result.insert(tuple_t(5));
		result.insert(tuple_t(6));
		result.insert(tuple_t(7));
		result.insert(tuple_t(8));
		result.insert(tuple_t(9));

		TESTASSERT(equal_tables(Union(t1, table_t()), t1));
		TESTASSERT(equal_tables(Union(table_t(), t1), t1));
		
//		print(Union(t1, t2));
		TESTASSERT(equal_tables(Union(t1, t2), result));
		
		test_decrement(Union(t1, t2));
	}

	test_function test1("UnionTest", foo);

	void bi()
	{
		typedef table<table_info<
			mpl::vector2<num, sec>, 
			mpl::vector1<num> 
			> > tab_type;

		typedef tab_type::value_type tuple_t;
		
		tab_type tab1, tab2, res;

		tab1.insert(tuple_t(0,0));
		tab1.insert(tuple_t(1,0));
		tab1.insert(tuple_t(2,0));
		tab1.insert(tuple_t(4,0));

		tab2.insert(tuple_t(1,1));
		tab2.insert(tuple_t(2,1));
		tab2.insert(tuple_t(3,1));
		tab2.insert(tuple_t(4,1));

		res.insert(tuple_t(0,0));
		res.insert(tuple_t(1,0));
		res.insert(tuple_t(2,0));
		res.insert(tuple_t(3,1));
		res.insert(tuple_t(4,0));

		TESTASSERT(equal_tables(Union(tab1,tab2),res));
//		print(Union(tab1,tab2));
	}

	test_function test1b("UnionBiTest",bi);

	void rangeTest()
	{
		using namespace test_zigzag;

//		union_t<distinct_t<table_t>, distinct_t<table_t> >::type u(distinct(table1()), distinct(table2()));
		union_t<table_t, table_t>::type u(table1(), table2());
		row<mpl::vector2<c1, c2> > subtuple(0, 1);

		TESTASSERT(equal_tables(
			u.equal_range(subtuple),
			range(u.lower_bound(subtuple), u.upper_bound(subtuple))));

		TESTASSERT(equal_tables(
			u.equal_range(subtuple),
            selection(u, lambda::_1[c1()]==0 && lambda::_1[c2()]==1/*And(eq<c1>(0), eq<c2>(1))*/)
			));
	}

	test_function test2("UnionRangeTest", rangeTest);

    void test_different_tables()
    {
		typedef table<table_info<mpl::vector1<num> > > t1_t;
		typedef table<table_info<mpl::list1<num> > > t2_t;

        t1_t t1;
        t2_t t2;

        print(Union(t1, t2));

    }
	test_function test_different_tables_fn("test_different_tables", test_different_tables);
}


