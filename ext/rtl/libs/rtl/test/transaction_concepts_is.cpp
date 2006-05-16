#include "pch.hpp"

#include "transaction_concepts.hpp"
#include "transaction_concepts_set.hpp"
#include "table_defs.hpp"

using namespace boost;

namespace tc
{
struct intersection_test : full_set_test
{
	typedef boost::rel::intersection_t<table_type_1,table_type_2>::type intersection_type;
	typedef boost::rel::isection_t<table_type_1,table_type_2>::type isection_type;
	typedef intersection_type result_type;
	typedef intersection_type::value_type tuple_r;
	typedef intersection_type::sort_list sort_list_r;
	BOOST_RTL_TABLE_2(table_type_r,intersection_type::field_list,sort_list_r);

	table_type_r res_;
	table_type_r mns_;
	table_type_r pls_;

	intersection_test()
	{
		// res_
		res_.insert(tuple_r(4,0));
		res_.insert(tuple_r(5,0));
		res_.insert(tuple_r(6,0));
		res_.insert(tuple_r(7,0));
		res_.insert(tuple_r(9,0));
		res_.insert(tuple_r(11,0));
		res_.insert(tuple_r(14,0));
		res_.insert(tuple_r(15,0));
		res_.insert(tuple_r(19,0));

		// mns_
		mns_.insert(tuple_r(4,0));
		mns_.insert(tuple_r(5,0));
		mns_.insert(tuple_r(6,0));
		mns_.insert(tuple_r(7,0));
		mns_.insert(tuple_r(9,0));
		mns_.insert(tuple_r(11,0));
		mns_.insert(tuple_r(15,0));

		// pls_
		pls_.insert(tuple_r(4,2));
		pls_.insert(tuple_r(8,2));
		pls_.insert(tuple_r(9,2));
		pls_.insert(tuple_r(12,2));
		pls_.insert(tuple_r(16,2));
		pls_.insert(tuple_r(17,2));
		pls_.insert(tuple_r(18,0));
	}

	void run()
	{
		intersection_type in = boost::rel::intersection(arg1_,arg2_);
		TESTASSERT(boost::rel::equal_tables(in,res_));
		simple_checks
		<
			result_type,
			table_type_r,
			table_type_r
//		>::perform_binary_test(in,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
		>::perform_cover_test(in,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
	}

	void irun()
	{
		isection_type in = boost::rel::isection(arg1_,arg2_);
		TESTASSERT(boost::rel::equal_tables(in,res_));
		simple_checks
		<
			isection_type,
			table_type_r,
			table_type_r
//		>::perform_binary_test(in,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
		>::perform_cover_test(in,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
	}

};

void isfoo()
{
	print_performance("before intersection test");
	intersection_test().run();
	print_performance("intersection test");
	intersection_test().irun();
	print_performance("isection test");
}

using namespace unit;

test_function istest("transaction_concepts_is", isfoo);


}
