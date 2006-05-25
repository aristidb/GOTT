#include "pch.hpp"

#include "test_function.hpp"
#include <boost/rtl/table_delta.hpp>
#include <boost/rtl/utils.hpp>
#include <boost/rtl/range_join_delta.hpp>
#include <boost/rtl/transaction.hpp>

using namespace unit;
using namespace std;
using namespace boost;
using namespace rel;

namespace range_join_cpp
{
	BOOST_RTL_DEFINE_COLUMN(int, a1);
	struct info_a : table_info<mpl::vector<a1> >{};
	typedef table<info_a> table_a;
	typedef table_a::value_type tuple_a;

	table_a create_a()
	{
		table_a t;
		t.insert(tuple_a(0));
		t.insert(tuple_a(1));
		t.insert(tuple_a(2));
		t.insert(tuple_a(3));
		t.insert(tuple_a(4));
		t.insert(tuple_a(5));
		return t;
	}

	BOOST_RTL_DEFINE_COLUMN(int, b1);
	BOOST_RTL_DEFINE_COLUMN(int, b2);
	struct info_b : table_info<mpl::vector<b1, b2> >{};
	typedef table<info_b> table_b;
	typedef table_b::value_type tuple_b;

	table_b create_b()
	{
		table_b t;
		t.insert(tuple_b(1, 1));
		t.insert(tuple_b(1, 2));
		t.insert(tuple_b(4, 1));
		t.insert(tuple_b(4, 2));
		return t;
	}

	typedef range_join_t<table_a, table_b, 
		lower_bound_t<mpl::vector<a1> >, 
		upper_bound_t<mpl::vector<a1> > >::type join_type;

	typedef range_join_t<table_b, table_a, 
		lower_bound_t<mpl::vector<b1> >, 
		upper_bound_t<mpl::vector<b1> > >::type reversed_join_type;

	template<class Join, class Sub, class Result>
	void lower_bound_test(const Join& jn, const Sub& sub, const Result& result)
	{
		typename Join::const_iterator it = jn.lower_bound(sub);
		if (it == jn.end())
		{
			TESTASSERT((compare_tuples<typename Join::sort_list>(result, row<mpl::vector3<a1, b1, b2> >(9, 9, 9)) == 0));
		}
		else
		{
            TESTASSERT(compare_tuples<typename Join::sort_list>(*it, result) == 0);
		}
	}

	template<class Join, class Sub, class Result>
	void upper_bound_test(const Join& jn, const Sub& sub, const Result& result)
	{
		typename Join::const_iterator it = jn.upper_bound(sub);
		if (it == jn.end())
		{
			TESTASSERT((compare_tuples<typename Join::sort_list>(result, row<mpl::vector3<a1, b1, b2> >(9, 9, 9)) == 0));
		}
		else
		{
            TESTASSERT(compare_tuples<typename Join::sort_list>(*it, result) == 0);
		}
	}

	template<class Join, class Sub>
	void equal_range_test(const Join& jn, const Sub& sub)
	{
		range_t<typename Join::const_iterator> r = jn.equal_range(sub);
		TESTASSERT(r.begin().base() == jn.lower_bound(sub) && r.end().base() == jn.upper_bound(sub));
	}

	void foo()
	{
		table_a a = create_a();
		table_b b = create_b();

		join_type jn = range_join(a, b, 
			lower_bound<mpl::vector<a1> >(), 
			upper_bound<mpl::vector<a1> >());

		reversed_join_type rjn = range_join(b, a, 
			lower_bound<mpl::vector<b1> >(), 
			upper_bound<mpl::vector<b1> >());

		join_type::const_iterator it = jn.begin();
		join_type::const_iterator end = jn.end();
		while (++it != end)
			;

		print(jn);
		print(rjn);

		lower_bound_test(jn, row<mpl::vector1<a1> >(1), row<mpl::vector3<a1, b1, b2> >(1, 1, 1));
		lower_bound_test(jn, row<mpl::vector1<a1> >(2), row<mpl::vector3<a1, b1, b2> >(4, 4, 1));
		lower_bound_test(jn, row<mpl::vector1<a1> >(5), row<mpl::vector3<a1, b1, b2> >(9, 9, 9));

		lower_bound_test(jn, row<mpl::vector2<a1, b1> >(1, 1), row<mpl::vector3<a1, b1, b2> >(1, 1, 1));
		lower_bound_test(jn, row<mpl::vector2<a1, b1> >(1, 2), row<mpl::vector3<a1, b1, b2> >(4, 4, 1));
		lower_bound_test(jn, row<mpl::vector2<a1, b1> >(4, 5), row<mpl::vector3<a1, b1, b2> >(9, 9, 9));

		lower_bound_test(jn, row<mpl::vector3<a1, b1, b2> >(1, 1, 1), row<mpl::vector3<a1, b1, b2> >(1, 1, 1));
		lower_bound_test(jn, row<mpl::vector3<a1, b1, b2> >(1, 1, 3), row<mpl::vector3<a1, b1, b2> >(4, 4, 1));
		lower_bound_test(jn, row<mpl::vector3<a1, b1, b2> >(4, 4, 3), row<mpl::vector3<a1, b1, b2> >(9, 9, 9));

		upper_bound_test(jn, row<mpl::vector1<a1> >(1), row<mpl::vector3<a1, b1, b2> >(4, 4, 1));
		upper_bound_test(jn, row<mpl::vector1<a1> >(4), row<mpl::vector3<a1, b1, b2> >(9, 9, 9));

		upper_bound_test(jn, row<mpl::vector2<a1, b1> >(1, 0), row<mpl::vector3<a1, b1, b2> >(1, 1, 1));
		upper_bound_test(jn, row<mpl::vector2<a1, b1> >(1, 1), row<mpl::vector3<a1, b1, b2> >(4, 4, 1));
		upper_bound_test(jn, row<mpl::vector2<a1, b1> >(4, 4), row<mpl::vector3<a1, b1, b2> >(9, 9, 9));

		upper_bound_test(jn, row<mpl::vector3<a1, b1, b2> >(1, 1, 2), row<mpl::vector3<a1, b1, b2> >(4, 4, 1));
		upper_bound_test(jn, row<mpl::vector3<a1, b1, b2> >(4, 4, 2), row<mpl::vector3<a1, b1, b2> >(9, 9, 9));
		
		for (int i = 0; i < 6; ++i)
		{
			equal_range_test(jn, row<mpl::vector<a1> >(i));

			for (int j = 0; j < 6; ++j)
			{
				equal_range_test(jn, row<mpl::vector<a1, b1> >(i, j));

				for (int k = 0; k < 4; ++k)
					equal_range_test(jn, row<mpl::vector<a1, b1, b2> >(i, j, k));
			}
		}

		// test delta

		print(a);
        print(b);

        transaction tr;
		
		tr.insert(a, tuple_a(6));
		TESTASSERT(count(delta(jn, tr).first) == 0);

		tr.insert(b, tuple_b(6, 1));
		tr.insert(b, tuple_b(6, 2));

        print(delta(jn, tr).first);
		TESTASSERT(count(delta(jn, tr).first) == 2);

		tr.insert(b, tuple_b(1, 3));
		tr.insert(b, tuple_b(1, 4));
		TESTASSERT(count(delta(jn, tr).first) == 4);

		print(delta(jn, tr).first);

		tr.rollback();

		it = jn.end();
		while (it != jn.begin())
			--it;
	}
	test_function _("range_join", foo);
}
