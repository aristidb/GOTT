#ifndef BOOST_RTL_TEST_DECREMENT_HPP_INCLUDED
#define BOOST_RTL_TEST_DECREMENT_HPP_INCLUDED

#include <vector>
#include <iostream>
#include <boost/rtl/utils.hpp>
#include <boost/rtl/table.hpp>
#include <boost/mpl/for_each.hpp>

template<class R>
	void test_decrement(const R& r)
{
	std::vector<typename R::const_iterator> v;
	
	typename R::const_iterator i = r.end();
	if (i != r.begin()) // DA
	{
		do
		{
			v.push_back(--i);
		}
		while (i != r.begin());
	}

	TESTASSERT(boost::rel::count(r) == static_cast<int>(v.size()));

	typename std::vector<typename R::const_iterator>::reverse_iterator j;
	for (i = r.begin(), j = v.rbegin(); i != r.end(); ++i, ++j)
	{
		if (i != *j)
		{
			std::cout << "should be:   ";
			
			boost::mpl::for_each<
				typename R::field_list>(boost::rel::print_field<typename R::value_type>(*i));

			std::cout << std::endl;
			std::cout << "actually is: ";
			
			boost::mpl::for_each<
				typename R::field_list>(boost::rel::print_field<typename R::value_type>(**j));

			std::cout << std::endl;
			TESTASSERT(false);
		}
	}
}

#endif//BOOST_RTL_TEST_DECREMENT_HPP_INCLUDED
