#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/table.hpp>
#include <boost/rtl/utils.hpp>
#include <boost/rtl/range.hpp>
#include "test_decrement.hpp"
#include <boost/rtl/crossproduct.hpp>

#include <boost/rtl/vector_impl.hpp>

#ifdef BOOST_RTL_USE_SET
#   include "indexed_set_impl.hpp"
#endif

using namespace boost;
using namespace rel;
using namespace unit;
//using namespace boost;

namespace table_cpp
{
	namespace TableTestNamespace
	{
		BOOST_RTL_DEFINE_COLUMN(int,c1);
		BOOST_RTL_DEFINE_COLUMN(int,c2);
		BOOST_RTL_DEFINE_COLUMN(int,c3);

        typedef boost::mpl::vector3<c1, c2, c3> list_type;
        typedef boost::mpl::vector3<c1, c2, c3> sort_type;

		typedef table<table_info<list_type, sort_type> > table_type;
	}

	using namespace TableTestNamespace;

	struct TableTest : public test
	{
		TableTest() : test("AAATableTest") 
		{
			insert(0, 0, 0);
			insert(0, 0, 1);
			insert(0, 1, 0);
			insert(0, 1, 1);
			insert(1, 0, 0);
			insert(1, 0, 1);
			insert(1, 1, 0);
			insert(1, 1, 1);
		}
		virtual void run();

	private:
		table_type tab;
		void insert(int i1, int i2, int i3)
		{
			table_type::value_type tup;
			BOOST_RTL_FIELD(tup, c1) = i1;
			BOOST_RTL_FIELD(tup, c2) = i2;
			BOOST_RTL_FIELD(tup, c3) = i3;
			tab.insert(tup);
		}
	};

	void TableTest::run()
	{
		
		//empty subtuple operations
        row<boost::mpl::vector0<> > tn;
		TESTASSERT(count(tab.equal_range(tn)) == 8);	
		TESTASSERT(tab.lower_bound(tn) == tab.begin());
		TESTASSERT(tab.upper_bound(tn) == tab.end());

		//partial subtuple opeartions
		row<boost::mpl::vector2<c1, c2> > tp;
		BOOST_RTL_FIELD(tp, c1) = 1;
		BOOST_RTL_FIELD(tp, c2) = 1;
		TESTASSERT(count(tab.equal_range(tp)) == 2);
		TESTASSERT(count(tab) == static_cast<int>(tab.size()));
		TESTASSERT(count(range(tab.lower_bound(tp),tab.upper_bound(tp))) == count(tab.equal_range(tp)));

		//decrementing
		test_decrement(tab);

		// check if it is default constructable
		table_type::const_iterator i;
		
		// copy constructor
		table_type other(tab);
		TESTASSERT(count(other) == count(tab));

		// assignment no assigment yet
		other = tab;
		TESTASSERT(count(other) == count(tab));

		const table_type& rtab = tab;
		rtab.equal_range(tp);

		tab.remove(tab.begin());
		TESTASSERT(count(tab.equal_range(tn)) == 7);	
        tab.insert(table_type::value_type(0,0,0));
		TESTASSERT(count(tab.equal_range(tn)) == 8);

		other.remove(tab.begin());
		other.insert(table_type::value_type(0,0,7));
		
		row<boost::mpl::vector1<c3> > tl;
		BOOST_RTL_FIELD(tl,c3) = 7;
		tab.update(tab.begin(),tl);

		TESTASSERT(count(other) == count(tab));

		TESTASSERT(equal_tables(other,tab));
	}
	TableTest test1;
}

namespace table_cpp
{
	BOOST_RTL_DEFINE_COLUMN(int, c);

	struct my_info : table_info<boost::mpl::vector1<c> >
	{};

	void less_test()
	{
		typedef table<my_info> table_t;
		std::cout << typeid(table_t).name() << std::endl;

		table_t t;
		for (int i = 0; i < 5; ++i)
			t.insert(table_t::value_type(i));

		print(t);
	}
	test_function test2("less_test", less_test);
}

#ifdef BOOST_RTL_USE_SET

namespace test_different_impl
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	BOOST_RTL_DEFINE_COLUMN(int, c2);

	typedef table<table_info<
		boost::mpl::vector<c1>,
		boost::mpl::vector<c1>,
		boost::mpl::vector<boost::mpl::vector<c1> >,
		sorted_strategy
	> > table1_type;

	typedef table<table_info<
		boost::mpl::vector<c2>,
		boost::mpl::vector<c2>,
		boost::mpl::vector<boost::mpl::vector<c2> >,
		indexed_set_strategy
	> > table2_type;

	void foo()
	{
		table1_type t1;
		table2_type t2;

		for (int i = 0; i < 3; ++i)
		{
			t1.insert(table1_type::value_type(i));
			t2.insert(table2_type::value_type(i));
		}
		print(cross_product(t1, t2));
	}
	test_function _("test_different_impl", foo);
}

#endif//BOOST_RTL_USE_SET
