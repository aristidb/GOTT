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

namespace trans_remove_test
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	typedef table<table_info<mpl::vector1<c1> > > table_t;

	void foo()
	{
		table_t t;
		transaction tr;

		TESTASSERT(tr.insert(t, row<mpl::vector1<c1> >(1)));
		TESTASSERT(tr.insert(t, row<mpl::vector1<c1> >(2)));
		TESTASSERT(tr.insert(t, row<mpl::vector1<c1> >(3)));
		TESTASSERT(tr.insert(t, row<mpl::vector1<c1> >(4)));
		TESTASSERT(tr.insert(t, row<mpl::vector1<c1> >(5)));

		TESTASSERT(tr.remove(t, row<mpl::vector1<c1> >(3)));
		TESTASSERT(count(modified(t, tr)) == 4);

		tr.commit();

		TESTASSERT(tr.remove(t.begin()));
		TESTASSERT(count(modified(t, tr)) == 3);
	}
	test_function test("trans_remove_test", foo);
}
