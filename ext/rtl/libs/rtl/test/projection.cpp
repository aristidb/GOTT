#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test.hpp"
#include <boost/rtl/tuple.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/projection.hpp>
#include "test_tables.hpp"
#include <boost/rtl/utils.hpp>
#include "test_function.hpp"
#include "test_decrement.hpp"

namespace projection_cpp
{
	using namespace boost;
	using namespace rel;
	using namespace unit;

	struct ProjectionTest : public test
	{
		typedef mpl::vector2<staff::ssn, staff::name> projection_list; 
		typedef mpl::vector1<staff::name> projection_list2; 

		ProjectionTest() : test("ProjectionTest") {}
		virtual void run();
	};

	void ProjectionTest::run()
	{
		staff::table t;
		// print(projection<projection_list>(t));
		// print(projection<projection_list2>(t));

		// assignable?
		projection_t<staff::table, projection_list>::type p(projection<projection_list>(t));
		projection_t<staff::table, projection_list>::type::const_iterator i(p.begin());
		i = p.begin();

		// default-constructable?
		projection_t<staff::table, projection_list>::type::const_iterator i2;

		test_decrement(p);
//		projection_t<staff::table, projection_list>::type::sort_list::error();
		TESTASSERT(count(p) == count(t));
	}

	ProjectionTest test;
}
