#ifndef BOOST_RTL_TRANSACTION_CONCEPTS_SET_HPP_INCLUDED
#define BOOST_RTL_TRANSACTION_CONCEPTS_SET_HPP_INCLUDED

#include "transaction_concepts.hpp"
#include "table_defs.hpp"

namespace tc
{

BOOST_RTL_DEFINE_COLUMN(Perfmeter,c1);
BOOST_RTL_DEFINE_COLUMN(Perfmeter,c2);
BOOST_RTL_DEFINE_COLUMN(Perfmeter,c3);
BOOST_RTL_DEFINE_COLUMN(Perfmeter,c4);

struct full_set_test
{
	typedef boost::mpl::vector2<c1, c2> f_list;
	typedef boost::mpl::vector1<c1> s_list;
	typedef boost::rel::row<f_list> tuple_1;
	typedef boost::rel::row<f_list> tuple_2;
//	typedef boost::rel::table<boost::rel::table_info<f_list,s_list> > table_type_1;
//	typedef boost::rel::table<boost::rel::table_info<f_list,s_list> > table_type_2;
	BOOST_RTL_TABLE_2(table_type_1,f_list,s_list);
	BOOST_RTL_TABLE_2(table_type_2,f_list,s_list);

	table_type_1 arg1_;
	table_type_1 mns1_;
	table_type_1 pls1_;
	table_type_2 arg2_;
	table_type_2 mns2_;
	table_type_2 pls2_;
	full_set_test()
	{
		// arg1_
		arg1_.insert(tuple_1(0,0));
		arg1_.insert(tuple_1(1,0));

		arg1_.insert(tuple_1(3,0));
		arg1_.insert(tuple_1(4,0));
		arg1_.insert(tuple_1(5,0));
		arg1_.insert(tuple_1(6,0));
		arg1_.insert(tuple_1(7,0));
		arg1_.insert(tuple_1(8,0));
		arg1_.insert(tuple_1(9,0));
		arg1_.insert(tuple_1(10,0));
		arg1_.insert(tuple_1(11,0));


		arg1_.insert(tuple_1(14,0));
		arg1_.insert(tuple_1(15,0));


		arg1_.insert(tuple_1(18,0));
		arg1_.insert(tuple_1(19,0));


		// arg2_
		arg2_.insert(tuple_2(4,1));
		arg2_.insert(tuple_2(5,1));
		arg2_.insert(tuple_2(6,1));
		arg2_.insert(tuple_2(7,1));

		arg2_.insert(tuple_2(9,1));

		arg2_.insert(tuple_2(11,1));
		arg2_.insert(tuple_2(12,1));
		arg2_.insert(tuple_2(13,1));
		arg2_.insert(tuple_2(14,1));
		arg2_.insert(tuple_2(15,1));

		arg2_.insert(tuple_2(17,1));

		arg2_.insert(tuple_2(19,1));
		arg2_.insert(tuple_2(20,1));

		arg2_.insert(tuple_2(22,1));
		arg2_.insert(tuple_2(23,1));

		// mns1_
		mns1_.insert(tuple_1(1,0));
		mns1_.insert(tuple_1(3,0));
		mns1_.insert(tuple_1(4,0));
		mns1_.insert(tuple_1(5,0));
		mns1_.insert(tuple_1(6,0));
		mns1_.insert(tuple_1(7,0));
		mns1_.insert(tuple_1(8,0));
		mns1_.insert(tuple_1(9,0));
		mns1_.insert(tuple_1(10,0));
		mns1_.insert(tuple_1(11,0));

		// mns2_
		mns2_.insert(tuple_2(4,1));
		mns2_.insert(tuple_2(5,1));
		mns2_.insert(tuple_2(6,1));
		mns2_.insert(tuple_2(7,1));
		mns2_.insert(tuple_2(12,1));
		mns2_.insert(tuple_2(13,1));
		mns2_.insert(tuple_2(14,1));
		mns2_.insert(tuple_2(15,1));
		mns2_.insert(tuple_2(20,1));
		mns2_.insert(tuple_2(22,1));

		// pls1_
		pls1_.insert(tuple_1(2,2));
		pls1_.insert(tuple_1(3,2));
		pls1_.insert(tuple_1(4,2));
		pls1_.insert(tuple_1(5,2));
		pls1_.insert(tuple_1(8,2));
		pls1_.insert(tuple_1(9,2));
		pls1_.insert(tuple_1(12,2));
		pls1_.insert(tuple_1(13,2));
		pls1_.insert(tuple_1(16,2));
		pls1_.insert(tuple_1(17,2));

		// pls2_
		pls2_.insert(tuple_2(4,3));
		pls2_.insert(tuple_2(6,3));
		pls2_.insert(tuple_2(8,3));
		pls2_.insert(tuple_2(10,3));
		pls2_.insert(tuple_2(12,3));
		pls2_.insert(tuple_2(14,3));
		pls2_.insert(tuple_2(16,3));
		pls2_.insert(tuple_2(18,3));
		pls2_.insert(tuple_2(20,3));
		pls2_.insert(tuple_2(21,3));
	}

};

}

#endif//BOOST_RTL_TRANSACTION_CONCEPTS_SET_HPP_INCLUDED
