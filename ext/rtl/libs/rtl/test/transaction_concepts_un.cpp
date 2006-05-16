#include "pch.hpp"

#include "transaction_concepts_set.hpp"

namespace tc
{
struct union_test : full_set_test
{
	typedef boost::rel::union_t<table_type_1,table_type_2>::type union_type;
	typedef union_type result_type;
	typedef union_type::value_type tuple_r;
	typedef union_type::sort_list sort_list_r;
	typedef boost::rel::table<boost::rel::table_info<union_type::field_list, sort_list_r> > table_type_r;

	table_type_r res_;
	table_type_r mns_;
	table_type_r pls_;

	union_test() : full_set_test()
	{

		// res_
		res_.insert(tuple_r(0,0));
		res_.insert(tuple_r(1,0));
		res_.insert(tuple_r(3,0));
		res_.insert(tuple_r(4,0));
		res_.insert(tuple_r(5,0));
		res_.insert(tuple_r(6,0));
		res_.insert(tuple_r(7,0));
		res_.insert(tuple_r(8,0));
		res_.insert(tuple_r(9,0));
		res_.insert(tuple_r(10,0));
		res_.insert(tuple_r(11,0));
		res_.insert(tuple_r(12,1));
		res_.insert(tuple_r(13,1));
		res_.insert(tuple_r(14,0));
		res_.insert(tuple_r(15,0));
		res_.insert(tuple_r(17,1));
		res_.insert(tuple_r(18,0));
		res_.insert(tuple_r(19,0));
		res_.insert(tuple_r(20,1));
		res_.insert(tuple_r(22,1));
		res_.insert(tuple_r(23,1));

		// mns_
		mns_.insert(tuple_r(1,0));
		mns_.insert(tuple_r(3,0));
		mns_.insert(tuple_r(4,0));
		mns_.insert(tuple_r(5,0));
		mns_.insert(tuple_r(6,0));
		mns_.insert(tuple_r(7,0));
		mns_.insert(tuple_r(8,0));
		mns_.insert(tuple_r(9,0));
		mns_.insert(tuple_r(10,0));
		mns_.insert(tuple_r(11,0));
		mns_.insert(tuple_r(12,1));
		mns_.insert(tuple_r(13,1));
		mns_.insert(tuple_r(17,1));
		mns_.insert(tuple_r(20,1));
		mns_.insert(tuple_r(22,1));

		// pls_
		pls_.insert(tuple_r(2,2));
		pls_.insert(tuple_r(3,2));
		pls_.insert(tuple_r(4,2));
		pls_.insert(tuple_r(5,2));
		pls_.insert(tuple_r(6,3));
		pls_.insert(tuple_r(8,2));
		pls_.insert(tuple_r(9,2));
		pls_.insert(tuple_r(10,3));
		pls_.insert(tuple_r(11,1));
		pls_.insert(tuple_r(12,2));
		pls_.insert(tuple_r(13,2));
		pls_.insert(tuple_r(16,2));
		pls_.insert(tuple_r(17,2));
		pls_.insert(tuple_r(20,3));
		pls_.insert(tuple_r(21,3));
	}
	void run()
	{
		union_type un = boost::rel::Union(arg1_,arg2_);
		TESTASSERT(boost::rel::equal_tables(un,res_));
//		transactions_checks<union_type>::perform_bin_test(un,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
		simple_checks
		<
			result_type,
			table_type_r,
			table_type_r
//		>::perform_binary_test(un,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
		>::perform_cover_test(un,mns1_,mns2_,pls1_,pls2_,mns_,pls_);

	}
};

void unfoo()
{
	print_performance("before union test");
	union_test().run();
	print_performance("union test");
}

using namespace unit;

test_function untest("transaction_concepts_un", unfoo);

}
