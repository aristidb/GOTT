#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test.hpp"
#include <boost/rtl/table.hpp>
#include <boost/rtl/utils.hpp>
#include "test_decrement.hpp"
#include <boost/rtl/merge.hpp>
#include <boost/rtl/crossproduct.hpp>
#include "test_decrement.hpp"

using namespace boost;
using namespace rel;

namespace qq
{
	using namespace unit;

	BOOST_RTL_DEFINE_COLUMN(std::string, fn)
	BOOST_RTL_DEFINE_COLUMN(std::string, ln)
	BOOST_RTL_DEFINE_COLUMN(int, bd)

	typedef mpl::vector3<fn, ln, bd> ta_fl;
	typedef mpl::vector2<ln, fn> ta_sl;
	typedef table<table_info<ta_fl,ta_sl> > TabA;
	typedef TabA::value_type TupA;

	BOOST_RTL_DEFINE_COLUMN(std::string, lnm);
	BOOST_RTL_DEFINE_COLUMN(std::string, bk);

	typedef mpl::vector2<lnm, bk> tb_fl;
	//typedef mpl::vector1<lnm> tb_sl;
	typedef mpl::vector2<lnm, bk> tb_sl; //AV
	typedef table<table_info<tb_fl,tb_sl> > TabB;
	typedef TabB::value_type TupB;

	struct MergeTest : public unit::test
	{
		typedef null_type no_prefix;
		typedef mpl::vector1<ln> simple_prefix;
		typedef mpl::vector1<lnm> another_simple_prefix;
		MergeTest() : unit::test("MergeTest") {}
		virtual void run();
	};

	void MergeTest::run()
	{

		TabA tabA; // sort_list is a key

		tabA.insert(TupA("Alexandr","Pushkin",1799));
		tabA.insert(TupA("Lev","Tolstoy",1828));
		tabA.insert(TupA("Fedor","Dostojevskiy",1821));
		tabA.insert(TupA("Michail","Lermontov",1814));
		tabA.insert(TupA("Nikolaj","Gogol",1809));
		tabA.insert(TupA("Alexey","Tolstoy",1817));

		TabB tabB; //sort_list is not a key

		tabB.insert(TupB("Gogol","Revisor"));
		tabB.insert(TupB("Tolstoy","Anna Karenina"));
		tabB.insert(TupB("Pushkin","Dubrovskiy"));
		tabB.insert(TupB("Tolstoy","Voskresenije"));
		tabB.insert(TupB("Gogol","Nos"));
		tabB.insert(TupB("Gogol","Portret"));
		tabB.insert(TupB("Pushkin","Metel"));

		typedef merge_t<TabB,TabA,0>::type mgt0;
		mgt0::const_iterator null; // default constructable
		TESTASSERT(equal_tables(merge<0>(tabA,tabB),cross_product(tabA,tabB)));

		typedef merge_t<TabA,TabB,1>::type mgt1;
		mgt1 mr(merge<1>(tabA,tabB));
		mgt1::const_iterator b = mr.begin();
		mgt1::const_iterator e = mr.end();
		mgt1::const_iterator bb = b;
		mgt1::const_iterator ee = e;

		for(int i = 0; i < count(mr); i++)
		{
			{
				for(int j=0; j < i; j++)
				{
					b++;
					e--;
				}
			}
			{
				for(int j=0; j < i; j++)
				{
					b--;
					e++;
				}
			}
			TESTASSERT(b == bb);
			TESTASSERT(e == ee);
		}

		test_decrement(merge<1>(tabA,tabB));
		typedef row<mpl::vector1<ln> > Sub;
		Sub sub;
		BOOST_RTL_FIELD(sub,ln) = std::string("Gogol");
		TESTASSERT(count(merge<1>(tabA,tabB).equal_range(sub)) == 3);
	}

	static MergeTest test;
}
