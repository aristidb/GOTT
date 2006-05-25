#include "pch.hpp"

//#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/rename.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/utils.hpp>
#include "test_tables.hpp"
#include <boost/rtl/crossproduct.hpp>

namespace rename_cpp
{
	using namespace boost;
	using namespace rel;
	using namespace unit;

	BOOST_RTL_DEFINE_COLUMN(int, c1);

	static void test1()
	{
		row<mpl::vector1<c1> > t1;
		BOOST_RTL_FIELD(t1, c1) = 5;

		typedef alias<c1> c2;
		row<mpl::vector1<c2> > t2;
		BOOST_RTL_FIELD(t2, c2) = BOOST_RTL_FIELD(t1, c1);

		TESTASSERT(BOOST_RTL_FIELD(t2, c2) == 5);
	}

	BOOST_RTL_DEFINE_COLUMN(float, c2);
	BOOST_RTL_DEFINE_COLUMN(char, c3);

	struct a;

	static void test2()
	{
		typedef mpl::vector3<c1, c2, c3> source;
		typedef mpl::vector3<
			alias<c1, a>, 
			alias<c2, a>, 
			alias<c3, a> > target;

		typedef mpl::vector3<
			boost::mpl::pair<c1, alias<c1, a> >,
			boost::mpl::pair<c2, alias<c2, a> >,
			boost::mpl::pair<c3, alias<c3, a> > 
			> target_pairs;
				
		BOOST_STATIC_ASSERT((list_equal<rename_pairs<source, a>::type, target_pairs>::value));
	}

	static void test3()
	{
		TESTASSERT(
			count(cross_product(staff::table(), auto_rename<a>(staff::table()))) == 
			count(staff::table())*count(staff::table())
		);
	}
	test_function t1("Rename1", test1);
	test_function t2("Rename2", test2);
	test_function t3("Rename3", test3);
}
