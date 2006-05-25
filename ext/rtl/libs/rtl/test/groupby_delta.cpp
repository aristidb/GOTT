#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/table_delta.hpp>
#include <boost/rtl/groupby_delta.hpp>
#include <boost/rtl/groupby_functors.hpp>
#include <boost/rtl/utils.hpp>

using namespace std;
using namespace unit;
using namespace boost;
using namespace rel;

namespace gby_delta_cpp
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	BOOST_RTL_DEFINE_COLUMN(int, c2);
	typedef mpl::vector2<c1, c2> columns;
	typedef mpl::vector1<c1> key_list;
	typedef mpl::vector1<sum<c2> > group_list;
	typedef table<table_info<columns> > table_t;
	typedef table_t::value_type tuple_t;
	typedef groupby_t<table_t, group_list, 1>::type groupby_type;

	template<class Rel, class Table>
		void materialize(const Rel& r, Table& t)
	{
		for (typename Rel::const_iterator it = 
			r.begin(); it != r.end(); ++it)
		{
			t.insert(*it);
		}
	}
	void foo()
	{
		table_t t;

		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 10; ++j)
				t.insert(tuple_t(i, j));
		}

//		groupby_type gby = groupby<key_list, group_list>(t);
		groupby_type gby = groupby<1, group_list>(t);
        print(gby);

		transaction tr;

		tr.insert(t, tuple_t(0, 11));
		tr.insert(t, tuple_t(10, 1));
		tr.remove(t, tuple_t(1, 9));

		//table<table_info<groupby_type::field_list> > m;
		//materialize(modified(gby, tr), m);


        traits<groupby_type>::delta_type d = delta(gby, tr);

        print(d.first);
        print(d.second);

        table_type<groupby_type>::type m;
        m.append(modified(gby, tr));
        
        print(m);

		tr.commit();

		TESTASSERT(equal_tables(m, groupby<1, group_list>(t)));
		TESTASSERT(equal_tables(gby, groupby<1, group_list>(t)));
	}

	test_function test("groupby_delta", foo);
}
