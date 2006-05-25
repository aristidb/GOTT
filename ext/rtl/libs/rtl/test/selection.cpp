#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test.hpp"
#include "test_tables.hpp"
#include <boost/rtl/selection.hpp>
#include <boost/rtl/select_functors.hpp>
#include <boost/rtl/utils.hpp>
#include "test_decrement.hpp"

namespace selection_cpp
{
    using namespace std;
	using namespace boost;
    using namespace rel;
    using namespace unit;
	using namespace lambda;

    struct SelectionTest : public test
    {
    	SelectionTest() : test("SelectionTest") {}
    	virtual void run();
    };


    void SelectionTest::run()
    {
	  	// print(selection(staff::table(), Or(ge<staff::year>(1970), ge<staff::year>(1970))));

    	int c = count(selection(staff::table(), 
            lambda::_1[staff::year()]>=1970 || lambda::_1[staff::year()]>=1970
			)); 
    	TESTASSERT(c == 2);

		TESTASSERT(equal_tables(
			staff::table(),
			selection(staff::table(), 
				lambda::_1[staff::year()]<=1970 || lambda::_1[staff::year()]>=1970 
				)
		));

		TESTASSERT(equal_tables(
			selection(staff::table(), lambda::_1[staff::year()]==1970),
			selection(staff::table(), 
				lambda::_1[staff::year()]<=1970 && lambda::_1[staff::year()]>=1970)
		));

  		c = count(selection(table<table_info<staff::table::field_list> >(), 
			lambda::_1[staff::year()]>=1970));

		TESTASSERT(c == 0);
		

  		// default-constructible?
  		// unary_iterator<selection_iterator<staff::table, eq<staff::year> > > i;

  		test_decrement(selection(staff::table(), lambda::_1[staff::year()]>=1970));
    }

	static SelectionTest test;
}
