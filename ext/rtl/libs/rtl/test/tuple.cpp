#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/tuple.hpp>

namespace tuple_cpp
{
	using namespace boost;
	using namespace rel;
	using namespace unit;

	BOOST_RTL_DEFINE_COLUMN(int, c1)
	BOOST_RTL_DEFINE_COLUMN(short, c2)

	typedef row<mpl::vector2<c1, c2> > tuple_type;

	void _test()
	{
		tuple_type t1(1, 2);

		tuple_type t2(t1);
		TESTASSERT(t2 == t1);

		t2 = t1;
		TESTASSERT(t2 == t1);
	}

	void _test_cctor()
	{
		typedef row<mpl::vector2<c1, c2> > tuple_t;

		tuple_t t1(1);
		TESTASSERT(BOOST_RTL_FIELD(t1, c1) == 1);
		TESTASSERT(BOOST_RTL_FIELD(t1, c2) == 0); // default

		tuple_t t2(t1);
		TESTASSERT(BOOST_RTL_FIELD(t2, c1) == 1);
		TESTASSERT(BOOST_RTL_FIELD(t2, c2) == 0); // default
	}

	test_function test("Tuple", _test);
	test_function test_cctor("Test copy-constructor", _test_cctor);
}
