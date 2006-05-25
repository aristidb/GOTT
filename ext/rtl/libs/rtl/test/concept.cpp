#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/table_delta.hpp>
#include <boost/rtl/utils.hpp>
#include "test_decrement.hpp"
#include <boost/rtl/range.hpp>
#include <boost/rtl/rtl.hpp>
#include <boost/rtl/select_functors.hpp>
#include <boost/rtl/groupby_functors.hpp>
#include <boost/rtl/range_selection.hpp>
#include <boost/rtl/range_join.hpp>
#include <boost/rtl/traverse.hpp>

//#include <boost/rtl/key.hpp>

//#include <boost/rtl/cover_delta.hpp>
#include <boost/rtl/cover.hpp>
#include <boost/rtl/cache.hpp>


using namespace std;
using namespace boost;
using namespace rel;
using namespace unit;
using namespace lambda;

namespace concept_cpp
{
	template<class FieldList> 
		void test_value_type(const row<FieldList>&)
	{}

	void test_iskey(const bool&)
	{}

	template<class T>
		void _do_test(T t)
	{
		// print(t);

		// iterator

		test_value_type(typename T::const_iterator::value_type());
		
		BOOST_STATIC_ASSERT((is_same<
			std::bidirectional_iterator_tag, 
			typename T::const_iterator::iterator_category>::value));

		BOOST_STATIC_ASSERT((is_same<
			ptrdiff_t, 
			typename T::const_iterator::difference_type>::value));

		BOOST_STATIC_ASSERT((is_same<
			T, 
			typename T::const_iterator::relation_type>::value));

		typename T::const_iterator i;
		typename T::const_iterator j(i);
		j = i;

		TESTASSERT(j == i);

		i = t.begin();
		TESTASSERT(i.relation() == &t);

		if (i != t.end())
		{
			i++;
			i--;
			TESTASSERT(i == t.begin());
			++i;
			--i;
			TESTASSERT(i == t.begin());
		}

		// forward reachability 
		i = t.begin();
		while(i != t.end()) ++i;

		// backward reachability
		i = t.end();
		while(i != t.begin()) --i;

		test_decrement(t);

		//relation

		typename T::const_iterator();
		test_value_type(typename T::value_type());
//		test_iskey(T::is_distinct);
		
		BOOST_STATIC_ASSERT((is_same<
			typename T::value_type, 
			typename T::const_iterator::value_type>::value));

		TESTASSERT(equal_tables(t, range(t.begin(), t.end())));

		row<mpl::vector0<> > null_tuple;
		TESTASSERT(t.equal_range(null_tuple).begin().base() == t.begin());
		TESTASSERT(t.equal_range(null_tuple).end().base() == t.end());
		TESTASSERT(t.lower_bound(null_tuple) == t.begin());
		TESTASSERT(t.upper_bound(null_tuple) == t.end());

		t.equal_range(row<typename T::sort_list>());

		//TESTASSERT(equal_tables(t, cover(t)));
	}

	
    struct dummy_traverser
	{
        template<class R, class Tag>
            void operator()(R&, Tag) const
        {}
    };

	template<class T>
		void do_test(T t)
	{
		_do_test(t);
        dummy_traverser tr;
		traverse(t, tr);
	}

	BOOST_RTL_DEFINE_COLUMN(int, c1);
	BOOST_RTL_DEFINE_COLUMN(int, c2);

	void concept_test()
	{
		typedef table<table_info<mpl::vector1<c1> > > table_t;
		typedef table_t::value_type tuple_t;

		typedef table<table_info<mpl::vector1<c2> > > table2_t;
		typedef table2_t::value_type tuple2_t;

		table_t t;
		table2_t t2;

		for (int i = 0; i < 10; ++i)
		{
			t.insert(tuple_t(i));
			t2.insert(tuple2_t(i));
		}

		do_test(t);

		do_test(projection<mpl::vector1<c1> >(t));
		do_test(selection(t, lambda::_1[c1()] <= 5));
		do_test(cross_product(t, auto_rename<void>(t)));
		do_test(Union(t, t));
		do_test(intersection(t, t));
		do_test(isection(t,t));
		do_test(difference(t,t));
		do_test(diff(t, t));
		do_test(equal_selection(t,tuple_t(5)));
		do_test(range_selection(t, lower_bound(tuple_t(5)), upper_bound(tuple_t(5))));
		do_test(join_eq<mpl::vector1<c1> >(t, t2));

		do_test(iterator_index<mpl::vector1<c1> >(t));
		do_test(key_index<mpl::vector1<c1> >(t));

		_do_test(range(t.begin(), t.end()));//skip traverse test
		do_test(groupby<1, mpl::vector1<sum<c1> > >(t));
		do_test(auto_rename<void>(t));
		do_test(manual_rename<mpl::vector1<c2> >(t));

		do_test(cover(t));
		do_test(merge<1>(t,t2));
		do_test(cache(t));

		do_test(join_eq<mpl::vector1<c1> >(t, auto_rename<void>(t)));
		do_test(join_le<mpl::vector1<c1> >(t, auto_rename<void>(t)));

		do_test(range_join(t, t2, 
			lower_bound<mpl::vector<c1> >(), 
			upper_bound<mpl::vector<c1> >())); 
	}

	test_function test("concept", concept_test);
}
