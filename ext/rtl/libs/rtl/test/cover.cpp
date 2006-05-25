#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include <boost/rtl/table_delta.hpp>
#include <boost/rtl/cover_delta.hpp>
#include "test_function.hpp"
#include "test_decrement.hpp"
#include <boost/rtl/zigzag_delta.hpp>
#include <boost/rtl/transaction.hpp>
#include <boost/rtl/key_index_delta.hpp>
#include <boost/rtl/iterator_index_delta.hpp>
#include "table_defs.hpp"
#include <boost/rtl/expression_registry.hpp>

namespace cp
{
	using namespace std;
	using namespace boost;
	using namespace rel;
	using namespace unit;

	BOOST_RTL_DEFINE_COLUMN(int, a1);
	BOOST_RTL_DEFINE_COLUMN(char, a2);
	BOOST_RTL_DEFINE_COLUMN(string, a3);

	typedef	mpl::vector3<a1, a2, a3> field_list_a;
	typedef mpl::vector2<a1, a2> sort_list_a;
	typedef table<table_info<field_list_a, sort_list_a> > table_a;
	typedef table_a::value_type tuple_a;
	typedef mpl::vector1<a1> sub_list;
	typedef row<sub_list> sub_tuple;
	typedef range_t<table_a::const_iterator> tblRange;

	typedef cover_t<table_a::value_type, table_a::sort_list, table_a::keys> cover_a;
	typedef range_t<cover_a::const_iterator> cvrRange;

	void _test1()
	{
		table_a tblA;
		tblA.insert(tuple_a(1,'a',"A"));
		tblA.insert(tuple_a(1,'b',"B"));
		tblA.insert(tuple_a(2,'c',"C"));

		cover_a cvrA = cover(tblA);
		test_decrement(cvrA);
		TESTASSERT(equal_tables(cvrA,tblA));

		sub_tuple sub(1);
		tblRange tblR(tblA.equal_range(sub));
		cvrRange cvrR(cvrA.equal_range(sub));
		TESTASSERT(equal_tables(cvrR,tblR));

		tblRange tblL(tblA.lower_bound(sub),tblA.end());
		cvrRange cvrL(cvrA.lower_bound(sub),cvrA.end());
		TESTASSERT(equal_tables(cvrL,tblL));

		tblRange tblH(tblA.begin(),tblA.upper_bound(sub));
		cvrRange cvrH(cvrA.begin(),cvrA.upper_bound(sub));
		TESTASSERT(equal_tables(cvrH,tblH));
	}

	typedef union_t<table_a,table_a>::type union_1;
	typedef union_t<union_1,union_1>::type union_2;
	typedef cover_t<union_1::value_type, union_1::sort_list, union_1::keys> cover_u1;
	typedef union_t<union_2,union_2>::type union_3;
	typedef cover_t<union_3::value_type, union_3::sort_list, union_3::keys> cover_u3;
//	typedef union_t<union_3,union_3>::type union_4;
	typedef union_t<cover_u3,cover_u3>::type union_c4;
	typedef union_t<union_c4,union_c4>::type union_c5;
//	typedef union_t<union_4,union_4>::type union_5;
	typedef union_t<cover_u1,cover_u1>::type union_c2;

	typedef mpl::vector1<a1> field_list_b;
	typedef mpl::vector1<a1> sort_list_b;
//	typedef table<table_info<field_list_b,sort_list_b> > table_b;
	BOOST_RTL_TABLE_2(table_b,field_list_b,sort_list_b);
	typedef table_b::value_type tuple_b;
	typedef union_t<table_b,table_b>::type union_1b;
	typedef cover_t<union_1b::value_type, union_1b::sort_list, union_1b::keys> cover_1b;


	void _test2()
	{
		table_a tblA;
		tblA.insert(tuple_a(1,'a',"A"));
		tblA.insert(tuple_a(1,'b',"B"));
		tblA.insert(tuple_a(2,'c',"C"));

		table_b tblB;
		tblB.insert(tuple_b(1));
		tblB.insert(tuple_b(2));

/*
		union_1 lev1 = Union(tblA,tblA);
		union_2 lev2 = Union(lev1,lev1);
		union_3 lev3 = Union(lev2,lev2);
		cover_u3 lev3c = cover(lev3);
		union_c4 lev4 = Union(lev3c,lev3c);
		union_c5 lev5 = Union(lev4,lev4);

//		union_4 lev4 = Union(lev3,lev3);
//		union_5 lev5 = Union(lev4,lev4);

//		TESTASSERT(equal_tables(lev3c,tblA));
		TESTASSERT(equal_tables(lev4,tblA));
		TESTASSERT(equal_tables(lev5,tblA));
*/
		union_1b lev1 = Union(tblB,tblB);
		cover_1b lev1b = cover(lev1);

//		union_c2 lev2 = Union(lev1c,lev1c);
//		TESTASSERT(equal_tables(lev2,tblA));

//		union_1 lev1 = Union(tblA,tblA);
//		cover_u1 lev1b = cover(lev1);

	}

	test_function test1("Cover Table",_test1);
	test_function test2("Cover Ladder", _test2);
}
namespace test_cover_index_reg
{
	using namespace boost;
	using namespace rel;
	using namespace unit;

	BOOST_RTL_DEFINE_COLUMN(int, c1);
	BOOST_RTL_DEFINE_COLUMN(int, c2);

	typedef mpl::vector2<c1, c2> columns;
	typedef mpl::vector2<c2, c1> index_columns;

	typedef table<table_info<columns> > table_t;
	typedef table_t::value_type tuple_t;

	void foo()
	{
		table_t t;

		for (int i = 0; i < 10; ++i)
			t.insert(tuple_t(i, 9 - i));

		key_index_t<cover_type<table_t>::type, index_columns>::type idx1 =
			key_index<index_columns>(cover(t)); 

		iterator_index_t<cover_type<table_t>::type, index_columns>::type idx2 =
			iterator_index<index_columns>(cover(t)); 

		expression_registry exprs;
		exprs.add(idx1);
		exprs.add(idx2);

		transaction tr;
		tr.insert(t, tuple_t(10, 10));
		tr.remove(t, tuple_t(10, 10));
		tr.insert(t, tuple_t(10, 11));
		tr.commit(exprs);

		TESTASSERT(count(t) == 11);
		TESTASSERT(equal_tables(t,cover(t)));
		TESTASSERT(equal_tables(idx1,idx2));
//		print(idx1);
//		print(idx2);
	}

	test_function test("test_cover_index_reg", foo);
}
