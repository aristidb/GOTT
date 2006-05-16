#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/table_delta.hpp>
#include <boost/rtl/extension_delta.hpp>
#include <boost/rtl/utils.hpp>
#include <boost/rtl/range_selection.hpp>
#include <string>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace rel;
using namespace std;

namespace extension_hpp
{
	BOOST_RTL_DEFINE_COLUMN(int, c1);
	struct my_info : table_info<mpl::vector1<c1> > {};
	typedef table<my_info> my_table;
	typedef my_table::value_type my_tuple;

	BOOST_RTL_DEFINE_COLUMN(int, c2);
	BOOST_RTL_DEFINE_COLUMN(string, c3);

	struct extender
	{
		typedef mpl::vector<c2, c3> field_list;
		
		template<class It>
			int get(const It& it, const c2&) const
		{
			return it.get(c1()) * it.get(c1()); 
		}
		template<class It>
			string get(const It& it, const c3&) const
		{
			return string("string-") + lexical_cast<string>(it.get(c1())); 
		}
	};

	void foo()
	{
		my_table t;

		for (int i = 0; i < 10; ++i)
			t.insert(my_tuple(i));

		print(t);
		print(extension(t, extender()));
		print(range_selection(extension(t, extender()), lower_bound(my_tuple(2)), upper_bound(my_tuple(7))));

		extension_t<my_table, extender>::type ext(t, extender());
		print(ext);

		transaction tr;
		tr.remove(t, my_tuple(0));
		tr.insert(t, my_tuple(10));

		print(delta(ext, tr).first); 
		print(delta(ext, tr).second); 

		TESTASSERT(equal_tables(
			modified(ext, tr),
			extension(modified(t, tr), extender())
			));
	}
	unit::test_function _("extension_test", foo);
}
