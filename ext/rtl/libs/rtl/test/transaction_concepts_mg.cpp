#include "pch.hpp"

#include "transaction_concepts.hpp"
#include "table_defs.hpp"

using namespace boost;

namespace tc
{

BOOST_RTL_DEFINE_COLUMN(Perfmeter,c1);
BOOST_RTL_DEFINE_COLUMN(Perfmeter,c2);
BOOST_RTL_DEFINE_COLUMN(Perfmeter,c3);
BOOST_RTL_DEFINE_COLUMN(Perfmeter,c4);

struct merge_test
{
	typedef mpl::vector2<c1, c2> f_list_1;
	typedef mpl::vector2<c3, c4> f_list_2;
	// no key but table itself

	BOOST_RTL_TABLE_1(table_type_1,f_list_1);
	BOOST_RTL_TABLE_1(table_type_2,f_list_2);

	typedef table_type_1::value_type tuple_1;
	typedef table_type_2::value_type tuple_2;

	table_type_1 arg1_;
	table_type_1 mns1_;
	table_type_1 pls1_;
	table_type_2 arg2_;
	table_type_2 mns2_;
	table_type_2 pls2_;

	typedef boost::rel::merge_t<table_type_1,table_type_2,1>::type merge_type;
	typedef merge_type::field_list field_list_r;
	typedef merge_type::sort_list sort_list_r;
	BOOST_RTL_TABLE_2(table_type_r,field_list_r,sort_list_r);
	typedef table_type_r::value_type tuple_r;

	table_type_r res_;
	table_type_r mns_;
	table_type_r pls_;

	// possible situations
	// t1t2m1m2p1p2[s1s2] 
	// t1t2m1m2p1p2[s1x2]
	// t1t2m1m2p1p2[x1s2]
	// t1t2m1m2p1p2[x1x2]

	// t1t2m1m2p1p2[s1s2]
	// t1t2m1m2p1p2[s1x2]
	// t1t2m1m2p1p2[x1s2]
	// t1t2m1m2p1p2[x1x2]
	// t1t2m1m2p1
	// t1t2m1m2p2
	// t1t2m1m2
	// t1t2m1p1
	// t1t2m1
	// t1t2m2p2
	// t1t2m2
	// t1t2
	// t1m1p1p2
	// t1m1p1
	// t1m1p2
	// t1m1
	// t1p2
	// t1
	// t2m2p1p2[s2s2s1s2]	(40)	// t2 had single entry, removed, single added to p1, single added to p2
	// t2m2p1p2[x2s2s1s2]	(39)	// t2 had multi etries, not all removed, single added to p1, single added to p2
	// t2m2p1p2[x2a2s1s2]	(38)	// t2 had multi etries, all removed, single added to p1, single added to p2
	// t2m2p1p2[s2s2s1x2]	(37)	// t2 had single entry, removed, single added to p1, multi added to p2
	// t2m2p1p2[x2s2s1x2]	(36)	// t2 had multi etries, not all removed, single added to p1, multi added to p2
	// t2m2p1p2[x2a2s1x2]	(35)	// t2 had multi etries, all removed, single added to p1, multi added to p2
	// t2m2p1p2[s2s2x1s2]	(34)	// t2 had single entry, removed, multi added to p1, single added to p2
	// t2m2p1p2[x2s2x1s2]	(33)	// t2 had multi etries, not all removed, multi added to p1, single added to p2
	// t2m2p1p2[x2a2x1s2]	(32)	// t2 had multi etries, all removed, multi added to p1, single added to p2
	// t2m2p1p2[s2s2x1x2]	(31)	// t2 had single entry, removed, multi added to p1, multi added to p2
	// t2m2p1p2[x2s2x1x2]	(30)	// t2 had multi etries, not all removed, multi added to p1, multi added to p2
	// t2m2p1p2[x2a2x1x2]	(29)	// t2 had multi etries, all removed, multi added to p1, multi added to p2
	// t2m2p1[s2s2s1]	(28)		// t2 had single entry, removed, single added to p1
	// t2m2p1[x2s2s1]	(27)		// t2 had milti entries, not all removed, single added to p1
	// t2m2p1[x2a2s1]	(26)		// t2 had multi etries, all removed, single added to p1
	// t2m2p1[s2s2x1]	(25)		// t2 had single entry, removed, , multi added to p1
	// t2m2p1[x2s2x1]   (24)		// t2 had milti entries, not all removed, multi added to p1
	// t2m2p1[x2a2x1]   (23)		// t2 had multi etries, all removed, multi added to p1
	// t2m2p2[s2s2s2]	(22)		// t2 had single entry, removed, single added
	// t2m2p2[x2s2s2]	(21)		// t2 had milti entries, not all removed, single added
	// t2m2p2[x2a2s2]	(20)        // t2 had multi entries, all removed, single added
	// t2m2p2[s2s2x2]	(19)		// t2 had single entry, removed multi added
	// t2m2p2[x2s2x2]	(18)		// t2 had milti entries, not all removed, multi added
	// t2m2p2[x2a2x2]	(17)        // t2 had multi entries, all removed, multi added
	// t2m2[s2s2]		(16)
	// t2m2[x2s2]		(15)		// t2 has milti entries, not all removed
	// t2m2[x2a2]		(14)        // t2 has multi entries, all removed
	// t2p1[s1s2]		(13)
	// t2p1[s1x2]		(12)
	// t2p1[x1s2]		(11)
	// t2p1[x1x2]		(10)	
	// t2[s2]			(9)
	// t2[x2]			(8)
	// p1p2[s1s2]		(7)
	// p1p2[s1x2]		(6)
	// p1p2[x1s2]		(5)
	// p1p2[x1x2]		(4)
	// p1[s1]			(3)
	// p1[x1]			(2)
	// p2[s2]			(1)
	// p2[x2]			(0)           
	
	merge_test()
	{
		// arg1_

		// arg2_
		arg2_.insert(tuple_2(8,1));
		arg2_.insert(tuple_2(8,3));
		arg2_.insert(tuple_2(9,5));
		arg2_.insert(tuple_2(10,1));
		arg2_.insert(tuple_2(10,3));
		arg2_.insert(tuple_2(11,1));
		arg2_.insert(tuple_2(11,3));
		arg2_.insert(tuple_2(12,5));
		arg2_.insert(tuple_2(13,5));
		arg2_.insert(tuple_2(14,1));
		arg2_.insert(tuple_2(14,3));
		arg2_.insert(tuple_2(15,1));
		arg2_.insert(tuple_2(15,3));
		arg2_.insert(tuple_2(16,5));
		arg2_.insert(tuple_2(17,1));
		arg2_.insert(tuple_2(17,3));
		arg2_.insert(tuple_2(18,1));
		arg2_.insert(tuple_2(18,3));
		arg2_.insert(tuple_2(19,5));
		arg2_.insert(tuple_2(20,1));
		arg2_.insert(tuple_2(20,3));
		arg2_.insert(tuple_2(21,1));
		arg2_.insert(tuple_2(21,3));
		arg2_.insert(tuple_2(22,5));
		arg2_.insert(tuple_2(23,1));
		arg2_.insert(tuple_2(23,3));
		arg2_.insert(tuple_2(24,1));
		arg2_.insert(tuple_2(24,3));
		arg2_.insert(tuple_2(25,5));
		arg2_.insert(tuple_2(26,1));
		arg2_.insert(tuple_2(26,3));
		arg2_.insert(tuple_2(27,1));
		arg2_.insert(tuple_2(27,3));
		arg2_.insert(tuple_2(28,5));
		arg2_.insert(tuple_2(29,1));
		arg2_.insert(tuple_2(29,3));
		arg2_.insert(tuple_2(30,1));
		arg2_.insert(tuple_2(30,3));
		arg2_.insert(tuple_2(31,5));
		arg2_.insert(tuple_2(32,1));
		arg2_.insert(tuple_2(32,3));
		arg2_.insert(tuple_2(33,1));
		arg2_.insert(tuple_2(33,3));
		arg2_.insert(tuple_2(34,5));


		// mns1_

		// mns2_
		mns2_.insert(tuple_2(14,1));
		mns2_.insert(tuple_2(14,3));
		mns2_.insert(tuple_2(15,3));
		mns2_.insert(tuple_2(16,5));
		mns2_.insert(tuple_2(17,1));
		mns2_.insert(tuple_2(17,3));
		mns2_.insert(tuple_2(18,3));
		mns2_.insert(tuple_2(19,5));
		mns2_.insert(tuple_2(20,1));
		mns2_.insert(tuple_2(20,3));
		mns2_.insert(tuple_2(21,3));
		mns2_.insert(tuple_2(22,5));
		mns2_.insert(tuple_2(23,1));
		mns2_.insert(tuple_2(23,3));
		mns2_.insert(tuple_2(24,3));
		mns2_.insert(tuple_2(25,5));
		mns2_.insert(tuple_2(26,1));
		mns2_.insert(tuple_2(26,3));
		mns2_.insert(tuple_2(27,3));
		mns2_.insert(tuple_2(28,5));
		mns2_.insert(tuple_2(29,1));
		mns2_.insert(tuple_2(29,3));
		mns2_.insert(tuple_2(30,3));
		mns2_.insert(tuple_2(31,5));
		mns2_.insert(tuple_2(32,1));
		mns2_.insert(tuple_2(32,3));
		mns2_.insert(tuple_2(33,3));
		mns2_.insert(tuple_2(34,5));

		// pls1_
		pls1_.insert(tuple_1(2,6));
		pls1_.insert(tuple_1(2,8));
		pls1_.insert(tuple_1(3,10));
		pls1_.insert(tuple_1(4,6));
		pls1_.insert(tuple_1(4,8));
		pls1_.insert(tuple_1(5,6));
		pls1_.insert(tuple_1(5,8));
		pls1_.insert(tuple_1(6,10));
		pls1_.insert(tuple_1(7,10));
		pls1_.insert(tuple_1(10,6));
		pls1_.insert(tuple_1(10,8));
		pls1_.insert(tuple_1(11,10));
		pls1_.insert(tuple_1(12,6));
		pls1_.insert(tuple_1(12,8));
		pls1_.insert(tuple_1(13,10));
		pls1_.insert(tuple_1(23,6));
		pls1_.insert(tuple_1(23,8));
		pls1_.insert(tuple_1(24,6));
		pls1_.insert(tuple_1(24,8));
		pls1_.insert(tuple_1(25,6));
		pls1_.insert(tuple_1(25,8));
		pls1_.insert(tuple_1(26,10));
		pls1_.insert(tuple_1(27,10));
		pls1_.insert(tuple_1(28,10));
		pls1_.insert(tuple_1(29,6));
		pls1_.insert(tuple_1(29,8));
		pls1_.insert(tuple_1(30,6));
		pls1_.insert(tuple_1(30,8));
		pls1_.insert(tuple_1(31,6));
		pls1_.insert(tuple_1(31,8));
		pls1_.insert(tuple_1(32,6));
		pls1_.insert(tuple_1(32,8));
		pls1_.insert(tuple_1(33,6));
		pls1_.insert(tuple_1(33,8));
		pls1_.insert(tuple_1(34,6));
		pls1_.insert(tuple_1(34,8));

		
		// pls2_
		pls2_.insert(tuple_2(0,7));
		pls2_.insert(tuple_2(0,9));
		pls2_.insert(tuple_2(1,11));
		pls2_.insert(tuple_2(4,7));
		pls2_.insert(tuple_2(4,9));
		pls2_.insert(tuple_2(5,11));
		pls2_.insert(tuple_2(6,7));
		pls2_.insert(tuple_2(6,9));
		pls2_.insert(tuple_2(7,11));
		pls2_.insert(tuple_2(17,7));
		pls2_.insert(tuple_2(17,9));
		pls2_.insert(tuple_2(18,7));
		pls2_.insert(tuple_2(18,9));
		pls2_.insert(tuple_2(19,7));
		pls2_.insert(tuple_2(19,9));
		pls2_.insert(tuple_2(20,11));
		pls2_.insert(tuple_2(21,11));
		pls2_.insert(tuple_2(22,11));
		pls2_.insert(tuple_2(29,7));
		pls2_.insert(tuple_2(29,9));
		pls2_.insert(tuple_2(30,7));
		pls2_.insert(tuple_2(30,9));
		pls2_.insert(tuple_2(31,7));
		pls2_.insert(tuple_2(31,9));
		pls2_.insert(tuple_2(32,11));
		pls2_.insert(tuple_2(33,11));
		pls2_.insert(tuple_2(34,11));
		// res_

		// mns_

		// pls_
		pls_.insert(tuple_r(4,6,7));
		pls_.insert(tuple_r(4,6,9));
		pls_.insert(tuple_r(4,8,7));
		pls_.insert(tuple_r(4,8,9));
		pls_.insert(tuple_r(5,6,11));
		pls_.insert(tuple_r(5,8,11));
		pls_.insert(tuple_r(6,10,7));
		pls_.insert(tuple_r(6,10,9));
		pls_.insert(tuple_r(7,10,11));
		pls_.insert(tuple_r(10,6,1));
		pls_.insert(tuple_r(10,8,1));
		pls_.insert(tuple_r(10,6,3));
		pls_.insert(tuple_r(10,8,3));
		pls_.insert(tuple_r(11,10,1));
		pls_.insert(tuple_r(11,10,3));
		pls_.insert(tuple_r(12,6,5));
		pls_.insert(tuple_r(12,8,5));
		pls_.insert(tuple_r(13,10,5));
		pls_.insert(tuple_r(24,6,1));
		pls_.insert(tuple_r(24,8,1));
		pls_.insert(tuple_r(27,10,1));
		pls_.insert(tuple_r(29,6,7));
		pls_.insert(tuple_r(29,8,7));
		pls_.insert(tuple_r(29,6,9));
		pls_.insert(tuple_r(29,8,9));
		pls_.insert(tuple_r(30,6,1));
		pls_.insert(tuple_r(30,8,1));
		pls_.insert(tuple_r(30,6,7));
		pls_.insert(tuple_r(30,8,7));
		pls_.insert(tuple_r(30,6,9));
		pls_.insert(tuple_r(30,8,9));
		pls_.insert(tuple_r(31,6,7));
		pls_.insert(tuple_r(31,8,7));
		pls_.insert(tuple_r(31,6,9));
		pls_.insert(tuple_r(31,8,9));
		pls_.insert(tuple_r(32,6,11));
		pls_.insert(tuple_r(32,8,11));
		pls_.insert(tuple_r(33,6,1));
		pls_.insert(tuple_r(33,8,1));
		pls_.insert(tuple_r(33,6,11));
		pls_.insert(tuple_r(33,8,11));
		pls_.insert(tuple_r(34,6,11));
		pls_.insert(tuple_r(34,8,11));

	}

	void run()
	{
		merge_type mg = boost::rel::merge<1>(arg1_,arg2_);
		TESTASSERT(boost::rel::equal_tables(mg,res_));
		simple_checks
		<
			merge_type,
			table_type_r,
			table_type_r
		>::perform_binary_test(mg,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
//		>::perform_cover_test(un,mns1_,mns2_,pls1_,pls2_,mns_,pls_);

	}

};

void mgfoo()
{
	print_performance("before merge test");
	merge_test().run();
	print_performance("merge test");
}

using namespace unit;

test_function mgtest("transaction_concepts_mg", mgfoo);

}
