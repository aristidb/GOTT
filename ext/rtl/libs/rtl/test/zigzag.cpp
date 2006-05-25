#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/table.hpp>
#include <boost/rtl/utils.hpp>
#include <boost/rtl/union.hpp>
#include <boost/rtl/intersection.hpp>
#include <boost/rtl/difference.hpp>

using namespace unit;
using namespace boost;
using namespace rel;

namespace zz
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	BOOST_RTL_DEFINE_COLUMN(int, c2);

	typedef table<table_info<mpl::vector2<c1, c2>, mpl::vector1<c1> > > table_t;
	
	void foo()
	{
		table_t t1, t2;
		t1.insert(table_t::value_type(0, 0));
		t2.insert(table_t::value_type(0, 1));

		TESTASSERT((*Union(t1, t2).begin())[c2()] == 0);
//		TESTASSERT((*intersection(t1, t2).begin())[c2()] == 0);
	}
	test_function test("zigzag-test", foo);
}
