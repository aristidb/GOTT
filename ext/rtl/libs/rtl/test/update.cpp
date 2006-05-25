#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include <boost/rtl/transaction.hpp>
#include <boost/rtl/table_delta.hpp>


#include <boost/rtl/utils.hpp>
#include "test_function.hpp"

namespace update_cpp
{
	using namespace boost;
	using namespace rel;

	BOOST_RTL_DEFINE_COLUMN(int, key);
	BOOST_RTL_DEFINE_COLUMN(double, val);
 
	struct my_info : table_info<
		mpl::vector<key, val>, 
		mpl::vector<key> 
		>{};

	typedef table<my_info> my_table;
	typedef my_table::value_type my_tuple; 
	typedef row<mpl::vector<key> > subtuple;

	template<class T>
	double find_val(const T& t, int k)
	{
		return t.lower_bound(subtuple(k)).get(val());
	}

	void foo()
	{
		my_table t;

		for (int i = 0; i < 10; ++i)
			t.insert(my_tuple(i, i));
		
		print(t);

		transaction tr;
		
		TESTASSERT(!tr.update(t, my_tuple(10, 10)));
		
		// update by value
		TESTASSERT(tr.update(t, my_tuple(5, 15)));
		TESTASSERT(find_val(delta(t, tr).first, 5) == 15);
		TESTASSERT(find_val(delta(t, tr).second, 5) == 5);
		TESTASSERT(find_val(modified(t, tr), 5) == 15);

		TESTASSERT(tr.update(t, my_tuple(5, 25)));
		TESTASSERT(find_val(delta(t, tr).first, 5) == 25);
		TESTASSERT(find_val(delta(t, tr).second, 5) == 5);
		TESTASSERT(find_val(modified(t, tr), 5) == 25);

		// update by iterator
		my_table::const_iterator it = t.lower_bound(subtuple(7));
		TESTASSERT(it != t.end());
		TESTASSERT(!tr.update(t, it, my_tuple(5, 15)));
		
		// key not changed
		TESTASSERT(tr.update(t, it, my_tuple(7, 17)));
		TESTASSERT(find_val(delta(t, tr).first, 7) == 17);
		TESTASSERT(find_val(delta(t, tr).second, 7) == 7);
		TESTASSERT(find_val(modified(t, tr), 7) == 17);

		// key changed
		TESTASSERT(tr.update(t, it, my_tuple(17, 17)));
		TESTASSERT(find_val(delta(t, tr).first, 17) == 17);
		TESTASSERT(find_val(delta(t, tr).second, 7) == 7);
		TESTASSERT(find_val(modified(t, tr), 17) == 17);

		tr.commit();
		print(t);
	}
	unit::test_function _("test_update", foo);
}// namespace update_cpp


