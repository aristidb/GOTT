#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/expression_registry.hpp>
#include <boost/rtl/key_index_delta.hpp>
#include <boost/rtl/iterator_index_delta.hpp>
#include <boost/rtl/table_delta.hpp>
#include <boost/rtl/cover_delta.hpp>
#include <boost/rtl/utils.hpp>

using namespace boost;
using namespace rel;
using namespace std;

namespace er
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	struct my_info : table_info<mpl::vector<c1> > {};
	typedef table<my_info> my_table;
	typedef my_table::value_type my_tuple;

	void foo()
	{
		my_table t;

		for (int i = 0; i < 10; ++i)
			t.insert(my_tuple(i));

		typedef key_index_t<my_table, mpl::vector1<c1> >::type k1_t; 
		typedef key_index_t<k1_t, mpl::vector1<c1> >::type k2_t; 
		typedef iterator_index_t<k1_t, mpl::vector1<c1> >::type i1_t; 
		typedef cover_type<i1_t>::type c1_t;
		typedef iterator_index_t<c1_t, mpl::vector1<c1> >::type i2_t;

		k1_t k1(t);
		k2_t k2(k1);
		i1_t i1(k1);
		c1_t c1(cover(i1));
		i2_t i2(c1);

		cout << "k1:" << (double)k1.id() << endl;
		cout << "k2:" << (double)k2.id() << endl;
		cout << "i1:" << (double)i1.id() << endl;
		cout << "i2:" << (double)i2.id() << endl;

		expression_registry exprs;

		exprs.add(k2);
		exprs.add(i2);
		
		exprs.print_ids();

		transaction tr;
		tr.insert(t, my_tuple(10));
		tr.insert(t, my_tuple(11));
		tr.remove(t, *t.lower_bound(my_tuple(3)));

		tr.commit(exprs);

		TESTASSERT(equal_tables(t, k1));
		TESTASSERT(equal_tables(t, k2));
		TESTASSERT(equal_tables(t, i1));
		TESTASSERT(equal_tables(t, i2));
	}
	unit::test_function _("expression_registry", foo);
}
