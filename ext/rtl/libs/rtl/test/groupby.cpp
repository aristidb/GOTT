#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/tuple.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/groupby.hpp>
#include <boost/rtl/groupby_functors.hpp>
#include <boost/rtl/utils.hpp>
#include "test_decrement.hpp"

namespace groupby_cpp
{
	using namespace std;
	using namespace boost;
	using namespace rel;

	BOOST_RTL_DEFINE_COLUMN(int, c1);
	BOOST_RTL_DEFINE_COLUMN(int, c2);

	void test()
	{
		sum<c1>::impl_type g1;
		counter::impl_type g2;

		TESTASSERT(g1.value() == 0);
		TESTASSERT(g2.value() == 0);

		row<mpl::vector2<c1, c2> > t;
		
		for (int i = 0; i < 5; i++)
		{
			BOOST_RTL_FIELD(t, c1) = i;
			BOOST_RTL_FIELD(t, c2) = i;
			g1(t);
			g2(t);
		}

		TESTASSERT(g1.value() == 10);
		TESTASSERT(g2.value() == 5);
	}

	void test1()
	{
		typedef mpl::vector2<c1, c2> s_list;
		typedef mpl::vector2<sum<c1>, counter> t_list;

		row<s_list> src;
		row<t_list> trg;

		for (int i = 0; i < 5; i++)
		{
			BOOST_RTL_FIELD(src, c1) = i;
			BOOST_RTL_FIELD(src, c2) = i;

			group_fields<t_list>(src, trg);
		}

		TESTASSERT(BOOST_RTL_FIELD(trg, sum<c1>) == 10);
		TESTASSERT(BOOST_RTL_FIELD(trg, counter) == 5);
	}

	void test2()
	{
		typedef mpl::vector2<c1, c2> field_list;

		typedef table<table_info<field_list> > table_type;
		typedef table_type::value_type tuple_type;

		table_type tab;
		tuple_type tup;

		int i;

		BOOST_RTL_FIELD(tup, c1) = 1;
		for (i = 0; i < 5; i++)
		{
			BOOST_RTL_FIELD(tup, c2) = i;
			tab.insert(tup);
		}

		BOOST_RTL_FIELD(tup, c1) = 2;
		for (i = 0; i < 5; i++)
		{
			BOOST_RTL_FIELD(tup, c2) = i * 2;
			tab.insert(tup);
		}

//		print(tab);

		// groupby

		typedef mpl::vector4<sum<c2>, counter, rel::max<c2>, rel::min<c2> > group_list;

		// AV: if I use sum<c2>, max<c2>, and min<c2> together, 
		// and groupby.cpp is not in the main directory,
		// INTERNAL COMPILER ERROR occurs in MSVC.

		// default constructible
		groupby_t<table_type, group_list, 1>::type::const_iterator it;

		groupby_t<table_type, group_list, 1>::type gb(groupby<1, group_list>(tab));

		TESTASSERT(count(groupby<1, group_list>(tab)) == 2);
		test_decrement(groupby<1, group_list>(tab));
	}

	unit::test_function _test("groupby", test);
	unit::test_function _test1("groupby1", test1);
	unit::test_function _test2("groupby2", test2);
}
