#include "pch.hpp"

#include "transaction_concepts.hpp"
#include "table_defs.hpp"

using namespace boost;

BOOST_RTL_DEFINE_COLUMN(Perfmeter,c1);
BOOST_RTL_DEFINE_COLUMN(Perfmeter,c2);
BOOST_RTL_DEFINE_COLUMN(Perfmeter,c3);
BOOST_RTL_DEFINE_COLUMN(Perfmeter,c4);

namespace tc
{

struct cross_product_test_full_sort_list_base
{

	typedef mpl::vector2<c1, c2> f_list_1;
	typedef mpl::vector2<c3, c4> f_list_2;
	typedef boost::rel::row<f_list_1> tuple_1;
	typedef boost::rel::row<f_list_2> tuple_2;

	BOOST_RTL_TABLE_1(table_type_1,f_list_1);
	BOOST_RTL_TABLE_1(table_type_2,f_list_2);

	typedef boost::rel::cross_product_t<table_type_1,table_type_2>::type result_type;
	typedef result_type::value_type tuple_r;
	typedef result_type::sort_list sort_list_r;
	BOOST_RTL_TABLE_2(table_type_r,result_type::field_list,sort_list_r);
	
	table_type_1 arg1_;
	table_type_1 mns1_;
	table_type_1 pls1_;
	table_type_2 arg2_;
	table_type_2 mns2_;
	table_type_2 pls2_;
	table_type_r res_;
	table_type_r mns_;
	table_type_r pls_;

	cross_product_test_full_sort_list_base()
	{
		// arg1_
		arg1_.insert(tuple_1(0,1));
		arg1_.insert(tuple_1(0,2));
		arg1_.insert(tuple_1(0,3));
		arg1_.insert(tuple_1(1,4));	

		// arg2_
		arg2_.insert(tuple_2(10,10));

		//res_
		res_.insert(tuple_r(0,1,10,10));
		res_.insert(tuple_r(0,2,10,10));
		res_.insert(tuple_r(0,3,10,10));
		res_.insert(tuple_r(1,4,10,10));
	};
	void run()
	{
		result_type xp = boost::rel::cross_product(arg1_,arg2_);
		simple_checks
		<
			result_type,
			table_type_r,
			table_type_r
//		>::perform_binary_test(xp,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
		>::perform_cover_test(xp,mns1_,mns2_,pls1_,pls2_,mns_,pls_);
	}
};

struct cross_product_test_1 : cross_product_test_full_sort_list_base
{
	cross_product_test_1() 
	{
		//mns1_
		mns1_.insert(tuple_1(1,4));
		//pls1_
		//mns2_
		//pls2_
		//mns_
		mns_.insert(tuple_r(1,4,10,10));
		//pls_
	}
};

struct cross_product_test_2 : cross_product_test_full_sort_list_base
{
	cross_product_test_2() 
	{
		//mns1_
		mns1_.insert(tuple_1(1,4));
		//pls1_
		//mns2_
		mns2_.insert(tuple_2(10,10));
		//pls2_
		//mns_
		mns_.insert(tuple_r(0,1,10,10));
		mns_.insert(tuple_r(0,2,10,10));
		mns_.insert(tuple_r(0,3,10,10));
		mns_.insert(tuple_r(1,4,10,10));
		//pls_
	}
};

void foo()
{
	print_performance("before cross_product");
	cross_product_test_full_sort_list_base().run();
	print_performance("cross_product test");
	cross_product_test_1().run();
	print_performance("cross_product test 1");
	cross_product_test_2().run();
	print_performance("cross_product test 2");
}

using namespace unit;

test_function test("transaction_concepts_xp", foo);

}
