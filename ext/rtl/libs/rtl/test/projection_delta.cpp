#include "pch.hpp"

#include <boost/rtl/traits.hpp>
#include <boost/rtl/projection_delta.hpp>
#include "table_defs.hpp"
#include "test_function.hpp"
#include <boost/rtl/expression_registry.hpp>
#include <boost/rtl/utils.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/table_delta.hpp>

namespace pd
{
	using namespace boost;
	using namespace rel;
	using namespace unit;

	BOOST_RTL_DEFINE_COLUMN(int,c1);
	BOOST_RTL_DEFINE_COLUMN(int,c2);

	typedef boost::mpl::vector2<c1,c2> flist;
	typedef boost::mpl::vector1<c1> xlist;
	BOOST_RTL_TABLE_1(tab_t,flist);
	typedef tab_t::value_type tup;

	BOOST_RTL_TABLE_1(res_t,xlist);
	typedef res_t::value_type rus;


	void foo()
	{
		tab_t tab;
		tab.insert(tup(1,0));
		tab.insert(tup(1,1));
		tab.insert(tup(1,2));
		tab.insert(tup(2,0));
		tab.insert(tup(2,1));
		tab.insert(tup(2,2));
		tab.insert(tup(3,0));
		tab.insert(tup(3,1));
		tab.insert(tup(3,2));

		res_t res;
		res.insert(rus(1));
		res.insert(rus(2));
		res.insert(rus(4));

		typedef projection_t<tab_t,xlist>::type pr_t;
//		typedef dprj_t<tab_t,xlist> pr_t;
		pr_t pr = projection<xlist>(tab);

		expression_registry exprs;
		exprs.add(pr);

		transaction tr;
		tr.insert(tab,tup(1,4));

		TESTASSERT(count(delta(pr,tr).first) == 0);
		TESTASSERT(count(delta(pr,tr).second) == 0);
		tr.insert(tab,tup(4,4));
		TESTASSERT(count(delta(pr,tr).first) == 1);
		TESTASSERT(count(delta(pr,tr).second) == 0);
		tr.remove(tab,tup(3,0));
		tr.remove(tab,tup(3,1));
		tr.remove(tab,tup(3,2));
		TESTASSERT(count(delta(pr,tr).first) == 1);
		TESTASSERT(count(delta(pr,tr).second) == 1);
		tr.commit();
		TESTASSERT(equal_tables(res,pr));
	}

	test_function _("projection_dlt", foo);
}

