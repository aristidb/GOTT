#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include "test_tables.hpp"
#include <boost/rtl/crossproduct.hpp>
#include <boost/rtl/utils.hpp>
#include "test_decrement.hpp"

namespace crossproduct_cpp
{
	using namespace std;
	using namespace boost;
	using namespace rel;
	using namespace unit;

	void _test()
	{
		staff::table t1;
		country::table t2;
		// print(cross_product(t1, t2)); 
		TESTASSERT(count(cross_product(t1, t2)) == 12);

		table<table_info<mpl::vector0<> > > null_table;
		TESTASSERT(count(cross_product(t1, null_table)) == 0);
		
		test_decrement(cross_product(t1, t2));
	}

	test_function test("CrossProductTest", _test);
}
