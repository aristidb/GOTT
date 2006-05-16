#ifndef BOOST_RTL_TEST_SUITE_HPP_INCLUDED
#define BOOST_RTL_TEST_SUITE_HPP_INCLUDED

#include <string>
#include <map>

namespace unit
{
	class test;

	class test_suite
	{
	public:
		static test_suite& instance();
		bool add_test(const std::string& name, test& test);
		bool run(const std::string& name);
		bool run();

	private:
		typedef std::map<std::string, test*> cont;
		test_suite() {}
		bool run_test(cont::iterator itr);
		cont tests_;
	};
}

#endif//BOOST_RTL_TEST_SUITE_HPP_INCLUDED
