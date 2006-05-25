#ifndef BOOST_RTL_TEST_ZIGZAG_HPP_INCLUDED
#define BOOST_RTL_TEST_ZIGZAG_HPP_INCLUDED

namespace test_zigzag
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	BOOST_RTL_DEFINE_COLUMN(int, c2);
	BOOST_RTL_DEFINE_COLUMN(int, c3);

	typedef boost::rel::table<boost::rel::table_info<
		boost::mpl::vector3<c1, c2, c3>
		> > table_t;

	typedef table_t::value_type tuple_t;

	inline table_t table1()
	{
		table_t result;
		result.insert(tuple_t(0, 0, 0));
		result.insert(tuple_t(0, 0, 1));
		result.insert(tuple_t(0, 1, 0));
		result.insert(tuple_t(0, 1, 1));
		result.insert(tuple_t(1, 0, 0));
		result.insert(tuple_t(1, 0, 1));
		return result;
	}

	inline table_t table2()
	{
		table_t result;
		result.insert(tuple_t(0, 1, 0));
		result.insert(tuple_t(0, 1, 1));
		result.insert(tuple_t(1, 0, 0));
		result.insert(tuple_t(1, 0, 1));
		result.insert(tuple_t(1, 1, 0));
		result.insert(tuple_t(1, 1, 1));
		return result;
	}
}

#endif
