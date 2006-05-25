#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include <boost/rtl/key_index.hpp>
#include "test.hpp"
#include <boost/rtl/utils.hpp>
#include <boost/rtl/invert.hpp>
#include <boost/rtl/range_selection.hpp>

using namespace std;
using namespace boost;
using namespace rel;
using namespace unit;

namespace key_index_cpp
{

BOOST_RTL_DEFINE_COLUMN(int,sn)
BOOST_RTL_DEFINE_COLUMN(std::string,qq)
	
typedef mpl::vector2<sn, qq> lst;
typedef table<table_info<lst> > full_table;
typedef full_table::value_type full_tuple;

typedef mpl::vector2<qq, sn> new_sort_list;
new_sort_list* sl = 0;

typedef full_table::sort_list old_sort_list;
old_sort_list* ol = 0;

typedef mpl::vector1<qq> partial_sort_list;

typedef row<partial_sort_list> SubTuple;

struct SecondaryIndexTest : public test
{
	SecondaryIndexTest() : test("SecondaryIndexTest") {}
	virtual void run();
};

void
SecondaryIndexTest::run()
{
	full_table t;
	t.insert(full_tuple(1,"zip11"));
	t.insert(full_tuple(1,"zip21"));
	t.insert(full_tuple(2,"zip21"));
	t.insert(full_tuple(0,"zip31"));

//	print(key_index<new_sort_list>(t,sl));
	SubTuple tup;
	BOOST_RTL_FIELD(tup,qq) = "zip21";
//	print(selection_eq(key_index(t, sl),tup));
//	print(key_index(key_index(t,sl),ol));
	TESTASSERT(equal_tables(
		t,
		key_index<old_sort_list>(key_index<new_sort_list>(t))
	));
	TESTASSERT(equal_tables(
		invert<new_sort_list,default_table_implementation>(t),
		key_index<new_sort_list>(t)
	));
	TESTASSERT(equal_tables(
		selection_eq(invert<new_sort_list,default_table_implementation>(t),tup),
		selection_eq(key_index<new_sort_list>(t),tup)
	));
	

//	print(key_index<new_sort_list>(key_index<old_sort_list>(key_index<new_sort_list>(t))));
}

static SecondaryIndexTest sitest;

}
