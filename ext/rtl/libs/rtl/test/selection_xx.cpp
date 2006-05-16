#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/table.hpp>
#include <boost/rtl/utils.hpp>
#include <boost/rtl/range_selection.hpp>
#include <boost/rtl/selection.hpp>
#include <boost/rtl/select_functors.hpp>

using namespace boost;
using namespace rel;
using namespace unit;
using namespace lambda;

BOOST_RTL_DEFINE_COLUMN(int, c1);
BOOST_RTL_DEFINE_COLUMN(int, c2);

typedef table<table_info<mpl::vector2<c1, c2> > > table_t;
typedef table_t::value_type tuple_t;

namespace selection_xx_cpp
{
	void foo()
	{
		table_t t;

		t.reserve(10000);
		for (int i = 0; i < 1000; i++)
			for (int j = 0; j < 10; j++)
				t.insert(tuple_t(i, j));

		TESTASSERT(equal_tables(
			selection_eq(t, row<mpl::vector1<c1> >(100)),
            selection(t, lambda::_1[c1()]==100/*eq<c1>(100)*/)));

		TESTASSERT(equal_tables(
			selection_lt(t, row<mpl::vector1<c1> >(100)),
            selection(t, lambda::_1[c1()]<100/*lt<c1>(100)*/)));

		TESTASSERT(equal_tables(
			selection_le(t, row<mpl::vector1<c1> >(100)),
            selection(t, lambda::_1[c1()]<=100/*le<c1>(100)*/)));

		TESTASSERT(equal_tables(
			selection_gt(t, row<mpl::vector1<c1> >(100)),
            selection(t, lambda::_1[c1()]>100/*gt<c1>(100)*/)));

		TESTASSERT(equal_tables(
			selection_ge(t, row<mpl::vector1<c1> >(100)),
            selection(t, lambda::_1[c1()]>=100/*ge<c1>(100)*/)));

		TESTASSERT(equal_tables(
			selection_eq(selection_eq(t, row<mpl::vector1<c1> >(100)), row<mpl::vector1<c1> >(100)),
			selection_eq(t, row<mpl::vector1<c1> >(100))
		));

		TESTASSERT(equal_tables(
			selection_le(selection_ge(t,row<mpl::vector1<c1> >(100)), row<mpl::vector1<c1> >(100)),
			selection_eq(t, row<mpl::vector1<c1> >(100))
		));
		// print(selection_le(selection_ge(t,row<mpl::vector1<c1> >(100)), row<mpl::vector2<c1, c2> >(100,1)));
		TESTASSERT(count(selection_lt(selection_ge(t,row<mpl::vector1<c1> >(100)), row<mpl::vector2<c1, c2> >(100,1)))==1);

	}

	test_function test("selection_xx", foo);
}
