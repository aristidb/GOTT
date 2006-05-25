#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include "test_decrement.hpp"
#include <boost/rtl/range.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/utils.hpp>

using namespace boost;
using namespace rel;
using namespace unit;

BOOST_RTL_DEFINE_COLUMN(int, c1);
typedef table<table_info<mpl::vector1<c1> > > table_t;
typedef table_t::value_type tuple_t;

namespace range_cpp
{

	void check1(const table_t& t1)	// DA, moved out to use const_iterator for the table.
	{
		TESTASSERT(equal_tables(
			range(t1.begin(), t1.end()),
			t1));
		TESTASSERT(equal_tables(
			range(t1.begin(), t1.begin()),
			range(t1.end(), t1.end())));
	}

	void check2(const table_t& t1, const table_t& t2)
	{
		test_decrement(range(t1.begin(), t1.end()));
	}

	void foo()
	{
		table_t t1, t2;
		int i;

		for (i = 0; i < 10; i++)
			t1.insert(tuple_t(i));

		check1(t1);

		for (i = 2; i < 7; i++)
			t2.insert(tuple_t(i));

		check2(t1,t2);
	}

	test_function test("range", foo);
}

