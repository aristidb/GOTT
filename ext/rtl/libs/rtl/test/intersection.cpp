#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/tuple.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/intersection.hpp>
#include "test_tables.hpp"
#include <boost/rtl/utils.hpp>
#include "test_zigzag.hpp"
#include <boost/rtl/selection.hpp>
#include <boost/rtl/select_functors.hpp>
#include "test_decrement.hpp"
#include <boost/rtl/isection.hpp>

using namespace boost;
using namespace rel;
using namespace unit;
using namespace lambda;

namespace intersection_cpp
{
	BOOST_RTL_DEFINE_COLUMN(int, num);
	BOOST_RTL_DEFINE_COLUMN(int, sec);

	void _test()
	{
		typedef table<table_info<
			mpl::vector1<num>
			> > table_t;

		typedef table_t::value_type tuple_t;

		table_t t1, t2, result;

		t1.insert(tuple_t(2));
		t1.insert(tuple_t(3));
		t1.insert(tuple_t(4));
		t1.insert(tuple_t(9));

		t2.insert(tuple_t(4));
		t2.insert(tuple_t(3));
		t2.insert(tuple_t(5));
		t2.insert(tuple_t(7));
		t2.insert(tuple_t(1));

		result.insert(tuple_t(3));
		result.insert(tuple_t(4));

		TESTASSERT(equal_tables(intersection(t1, t2), result));
		print(isection(t1,t2));
		TESTASSERT(equal_tables(isection(t1,t2),result));
		test_decrement(isection(t1,t2));
	}

	test_function test("IntersectionTest", _test);


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

		res.insert(tuple_t(1,0));
		res.insert(tuple_t(2,0));
		res.insert(tuple_t(4,0));

		TESTASSERT(equal_tables(res,intersection(tab1,tab2)));
//		print(intersection(tab1,tab2));
	}

	test_function test1b("IntersectionBiTest",bi);


	void rangeTest()
	{
		using namespace test_zigzag;

		intersection_t<table_t, table_t>::type i(table1(), table2());
		isection_t<table_t,table_t>::type ii(table1(), table2());
//		print(i);

		row<mpl::vector2<c1, c2> > subtuple(0, 1);
		row<mpl::vector1<c1> > sub(1);

		TESTASSERT(equal_tables(
			i.equal_range(subtuple),
			range(i.lower_bound(subtuple), i.upper_bound(subtuple))));

		TESTASSERT(equal_tables(
			i.equal_range(sub),
			range(i.lower_bound(sub), i.upper_bound(sub))));

		TESTASSERT(equal_tables(
			ii.equal_range(subtuple),
			range(ii.lower_bound(subtuple), ii.upper_bound(subtuple))));

		TESTASSERT(equal_tables(
			ii.equal_range(sub),
			range(i.lower_bound(sub), i.upper_bound(sub))));

		TESTASSERT(equal_tables(
				ii.equal_range(subtuple),
				i.equal_range(subtuple)
			));

		TESTASSERT(equal_tables(
			i.equal_range(subtuple),
			selection(i, lambda::_1[c1()]==0 && lambda::_1[c2()]==1)
			));
	}

	test_function test2("IntersectionRangeTest", rangeTest);

	void isectionTest()
	{
		using namespace test_zigzag;
		isection_t<table_t,table_t>::type i(table1(),table2());

	}

	test_function test3("Isection Test",isectionTest);
}

