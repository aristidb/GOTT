#include "pch.hpp"

#include "transaction_concepts.hpp"
#include "transaction_concepts_set.hpp"
#include "table_defs.hpp"

using namespace boost;

namespace tc
{

struct difference_test : full_set_test
{
	
	typedef boost::rel::difference_t<table_type_1,table_type_2>::type difference_type;
	typedef boost::rel::diff_t<table_type_1,table_type_2>::type diff_type;
	typedef difference_type result_type;
	typedef difference_type::value_type tuple_r;
	typedef difference_type::sort_list sort_list_r;
//	typedef boost::rel::table<boost::rel::table_info<difference_type::field_list,sort_list_r> > table_type_r;
	BOOST_RTL_TABLE_2(table_type_r,difference_type::field_list,sort_list_r);

	table_type_r res_;
	table_type_r mns_;
	table_type_r pls_;
	
	difference_test()
	{
		// res
		res_.insert(tuple_r(0,0));
		res_.insert(tuple_r(1,0));
		res_.insert(tuple_r(3,0));
		res_.insert(tuple_r(8,0));
		res_.insert(tuple_r(10,0));
		res_.insert(tuple_r(18,0));

		// min
		mns_.insert(tuple_r(1,0));
		mns_.insert(tuple_r(3,0));
		mns_.insert(tuple_r(8,0));
		mns_.insert(tuple_r(10,0));
		mns_.insert(tuple_r(18,0));

		// pls
		pls_.insert(tuple_r(2,2));
		pls_.insert(tuple_r(3,2));
		pls_.insert(tuple_r(5,2));
		pls_.insert(tuple_r(13,2));
		pls_.insert(tuple_r(15,0));
	}

	void run()
	{
		difference_type df = boost::rel::difference(arg1_,arg2_);
		TESTASSERT(boost::rel::equal_tables(df,res_));
		simple_checks
		<
			result_type,
			table_type_r,
			table_type_r
		>::perform_binary_test(df,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
//		>::perform_cover_test(df,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
	}

	void irun()
	{
		diff_type df = boost::rel::diff(arg1_,arg2_);
		TESTASSERT(boost::rel::equal_tables(df,res_));
		simple_checks
		<
			diff_type,
			table_type_r,
			table_type_r
//		>::perform_binary_test(df,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
		>::perform_cover_test(df,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
	}
};

void dffoo()
{
	print_performance("before difference test");
	difference_test().run();
	print_performance("difference test");
	difference_test().irun();
	print_performance("diff test");
}

using namespace unit;

test_function dftest("transaction_concepts_df", dffoo);


}

