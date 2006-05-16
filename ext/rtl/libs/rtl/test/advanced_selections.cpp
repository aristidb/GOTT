#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/utils.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/range_selection.hpp>
#include <boost/rtl/selection.hpp>
#include <boost/rtl/select_functors.hpp>
#include <boost/rtl/iterator_index.hpp>
#include <boost/rtl/union.hpp>
#include <boost/rtl/intersection.hpp>

using namespace std;
using namespace boost;
using namespace rel;
using namespace unit;
using namespace lambda;

BOOST_RTL_DEFINE_COLUMN(int, ky);
BOOST_RTL_DEFINE_COLUMN(int, c1);
BOOST_RTL_DEFINE_COLUMN(int, c2);

typedef table
<
	table_info
	<
		mpl::vector3<ky, c1, c2>, 
		mpl::vector1<ky>
	> 
> table_t;

typedef table_t::value_type tuple_t;

namespace advanced_selections_cpp
{
	void foo()
	{
		table_t t;
		int k = 0; // primary ky

		t.reserve(25);

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
				t.insert(tuple_t(k++, i, j * 10));
		}

		iterator_index_t<table_t, mpl::vector2<c1,ky> >::type index1(t);
		iterator_index_t<table_t, mpl::vector2<c2,ky> >::type index2(t);
		
		// SELECT FROM t WHERE c1 = 3 OR c2 = 20
		TESTASSERT(equal_tables(
			Union
			(
				iterator_index<mpl::vector1<ky> >(selection_eq(index1, row<mpl::vector1<c1> >(3))),
				iterator_index<mpl::vector1<ky> >(selection_eq(index2, row<mpl::vector1<c2> >(20)))
			),
			selection(t, lambda::_1[c1()] == 3 || lambda::_1[c2()] == 20)
		));


		// SELECT FROM t WHERE c1 = 3 AND c2 = 20
		TESTASSERT(equal_tables(
			intersection
			(
				iterator_index<mpl::vector1<ky> >(selection_eq(index1, row<mpl::vector1<c1> >(3))),
				iterator_index<mpl::vector1<ky> >(selection_eq(index2, row<mpl::vector1<c2> >(20)))
			),
			selection(t, lambda::_1[c1()] == 3 && lambda::_1[c2()] == 20)
		));

		TESTASSERT(equal_tables(
			intersection(
				iterator_index<mpl::vector1<ky> >(selection_eq(index1, row<mpl::vector1<c1> >(3))),
				iterator_index<mpl::vector1<ky> >(selection_eq(index2, row<mpl::vector1<c2> >(20)))
			),
			selection(t, lambda::_1[c1()] == 3 && lambda::_1[c2()] == 20)
		));

	}

	test_function test("advanced_selections", foo);
}
