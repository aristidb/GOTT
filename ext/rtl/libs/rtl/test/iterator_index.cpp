#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include <boost/rtl/iterator_index.hpp>
#include "test_tables.hpp"
#include "test.hpp"
#include <boost/rtl/utils.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/invert.hpp>
#include "test_decrement.hpp"

namespace iterator_index_cpp
{
	using namespace boost;
	using namespace std;
	using namespace rel;
	using namespace unit;

	struct PrimaryIndexTest : public test
	{
		PrimaryIndexTest() : test("PrimaryIndexTest") {}
		virtual void run();
	};

	// I added "sn" here to fix test.  
	// It should NOT compile (but compiles) without it!  AV
	typedef mpl::vector2<myspace::qq, myspace::sn> new_sort_list;
	new_sort_list* sl = 0;

	typedef myspace::table::sort_list old_sort_list;
	old_sort_list* ol = 0;

	typedef row<mpl::vector1<myspace::qq> > SubTuple;

	void
	PrimaryIndexTest::run()
	{
		myspace::table t;
		iterator_index_t<myspace::table,new_sort_list>::type::const_iterator it; // default constructible

		test_decrement(iterator_index<new_sort_list>(t));

	//	print(iterator_index<new_sort_list>(t,sl));

		TESTASSERT(equal_tables(
			iterator_index<new_sort_list>(t, sl),
	//		invert_table<new_sort_list, sorted_vector<row<myspace::table::value_type::field_list, new_sort_list> > >(t)));
			invert<new_sort_list, default_table_implementation>(t)));

		SubTuple tup;
		BOOST_RTL_FIELD(tup,myspace::qq) = "zip21";

		print(iterator_index<new_sort_list>(t));
		print(iterator_index<new_sort_list>(t).equal_range(tup));

		TESTASSERT(count(iterator_index<new_sort_list>(t).equal_range(tup)) == 2);

	//	print(iterator_index(iterator_index(t,sl),ol));

		TESTASSERT(equal_tables(
			t,
			iterator_index(iterator_index(t,sl),ol)
		));

	//	print(iterator_index<new_sort_list>(iterator_index<old_sort_list>(iterator_index<new_sort_list>(t))));
	}

	static PrimaryIndexTest pitest;


}
