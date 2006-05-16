#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/tuple.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/difference.hpp>
#include "test_tables.hpp"
#include <boost/rtl/utils.hpp>
#include <boost/rtl/rename.hpp>
#include "test_decrement.hpp"
#include "test_zigzag.hpp"
#include <boost/rtl/selection.hpp>
#include <boost/rtl/select_functors.hpp>

using namespace boost;
using namespace rel;
using namespace unit;
using namespace lambda;

namespace difference_cpp
{
	BOOST_RTL_DEFINE_COLUMN(int, num1);
	BOOST_RTL_DEFINE_COLUMN(int, num2);

	void testDifference()
	{
		typedef table<table_info<
			mpl::vector1<num1>
			> > table1_t;

		typedef table<table_info<
			mpl::vector1<num2>
			> > table2_t;

		typedef table1_t::value_type tuple1_t; 
		typedef table2_t::value_type tuple2_t;

		table1_t m_t1;
		table2_t m_t2;

		m_t1.insert(tuple1_t(2));
		m_t1.insert(tuple1_t(3));
		m_t1.insert(tuple1_t(4));
		m_t1.insert(tuple1_t(9));

		std::cout << "m_t1" << std::endl;
		print(m_t1);
		m_t2.insert(tuple2_t(4));
		m_t2.insert(tuple2_t(3));
		m_t2.insert(tuple2_t(5));
		m_t2.insert(tuple2_t(7));
		m_t2.insert(tuple2_t(1));
		std::cout << "m_t2" << std::endl;
		print(m_t2);
		
//		std::cout << "difference(m_t1,m_t2)" << std::endl;
//		print(difference(m_t1, manual_rename<tuple1_t::field_list, table2_t>(m_t2)));

		print(difference(m_t1, m_t1));

		TESTASSERT(count(difference(m_t1, m_t1)) == 0);
		TESTASSERT(equal_tables(
			difference(m_t1,difference(m_t1,m_t1)),
			m_t1
		));
		
		test_decrement(difference(m_t1, manual_rename<tuple1_t::field_list, table2_t>(m_t2)));
		test_decrement(difference(m_t2, manual_rename<tuple2_t::field_list, table1_t>(m_t1)));
	}

	test_function test("Difference",  testDifference);
}

namespace difference_cpp
{
	void _rangeTest(int n1, int n2)
	{
		using namespace test_zigzag;

		difference_t<table_t, table_t>::type d(table1(), table2());
		row<mpl::vector2<c1, c2> > subtuple(n1, n2);

//		print(d);

//		std::cout << "equal_range " << n1 << " " << n2 << std::endl;
//		print(d.equal_range(subtuple));

//		std::cout << "selection(d, And(eq<c1>(n1), eq<c2>(n2))) " << n1 << " " << n2 << std::endl;
//		print(selection(d, And(eq<c1>(n1), eq<c2>(n2))));

		TESTASSERT(equal_tables(
			d.equal_range(subtuple),
			range(d.lower_bound(subtuple), d.upper_bound(subtuple))));

		TESTASSERT(equal_tables(
			d.equal_range(subtuple),
			selection(d, lambda::_1[c1()] == n1 && lambda::_1[c2()] == n2)
			));
	}

	void rangeTest()
	{
		_rangeTest(0, 0);
		_rangeTest(0, 1);
	}
	
	test_function test2("DifferenceRangeTest", rangeTest);
}
