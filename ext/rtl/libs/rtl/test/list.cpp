#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/list.hpp>
#include <boost/static_assert.hpp>
#include <iostream>

using namespace boost;
using namespace rel;
using namespace unit;
using namespace std;

namespace list_cpp
{
	typedef	mpl::vector2<int, short> l1;
	typedef	mpl::vector2<float, double> l2;
	typedef	mpl::vector4<int, short, float, double> l3;
	typedef mpl::vector3<int, short, int> l5;
	typedef mpl::vector2<int, short> l6;
	typedef mpl::vector1<int> l7;
	typedef mpl::vector1<double> l8;

	void foo()
	{
		// concat
		BOOST_STATIC_ASSERT((list_equal<l3, concat<l1, l2>::type>::value));

		// trim_list
		BOOST_STATIC_ASSERT((list_equal<list_prefix<l5, 2>::type, l6>::value));

		BOOST_STATIC_ASSERT((list_equal<list_prefix<mpl::vector0<>, 0>::type, mpl::vector0<> >::value));

		// contains
		BOOST_STATIC_ASSERT((list_contains<l3, double>::value));
		BOOST_STATIC_ASSERT((!list_contains<l3, char>::value));

		// list_difference
		BOOST_STATIC_ASSERT((list_equal<list_difference<l2,l1>::type, l2>::value));
		BOOST_STATIC_ASSERT((list_equal<list_difference<mpl::vector0<>,l1>::type,mpl::vector0<> >::value));

		// intersect
		BOOST_STATIC_ASSERT((list_equal<list_intersection<mpl::vector0<>,mpl::vector0<> >::type,mpl::vector<> >::value));
		BOOST_STATIC_ASSERT((list_equal<list_intersection<mpl::vector0<>,l1>::type,mpl::vector0<> >::value));
		BOOST_STATIC_ASSERT((list_equal<list_intersection<l1,l1>::type,l1>::value));
		BOOST_STATIC_ASSERT((list_equal<list_intersection<l3,l1>::type,l1>::value));
		BOOST_STATIC_ASSERT((list_equal<list_intersection<l1,l3>::type,l1>::value));
		BOOST_STATIC_ASSERT((list_equal<list_intersection<l1,l2>::type,mpl::vector0<> >::value));

		// union
		BOOST_STATIC_ASSERT((list_equal<list_union<mpl::vector0<> ,l2>::type,l2>::value));
		BOOST_STATIC_ASSERT((list_equal<list_union<l7,l1>::type,l1>::value));
	}

	static test_function test("ListTest", foo);
}

namespace list_contains_all_test
{
	enum {test1 = list_contains_all<mpl::vector2<void, short>, mpl::vector1<short> >::value};
	BOOST_STATIC_ASSERT(test1);
	
	enum {test2 = !list_contains_all<mpl::vector2<void, short>, mpl::vector1<double> >::value};
	BOOST_STATIC_ASSERT(test2);

	enum {test5 = !list_contains_all<mpl::vector1<double>, mpl::vector2<void, short> >::value};
	BOOST_STATIC_ASSERT(test5);

	enum {test3 = list_contains_all<mpl::vector2<void, short>, mpl::vector0<> >::value};
	BOOST_STATIC_ASSERT(test3);

	enum {test4 = list_contains_all<
		mpl::vector2<void, short>, 
		mpl::vector2<void, short> 
		>::value};

	BOOST_STATIC_ASSERT(test4);
}
