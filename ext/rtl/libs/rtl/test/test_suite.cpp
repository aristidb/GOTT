#include "pch.hpp"

#pragma warning(disable:4786)

#include <iostream>
#include "test_suite.hpp"
#include "test.hpp"
 
using namespace unit;

test_suite& test_suite::instance()
{
	static test_suite inst;
	return inst;
}

bool test_suite::add_test(const std::string& name, test& t)
{
	return tests_.insert(std::make_pair(name, &t)).second;
}

bool test_suite::run(const std::string& name)
{
	cont::iterator itr = tests_.find(name);

	if (itr == tests_.end())
		return false;

	return run_test(itr);
}

bool test_suite::run()
{
	bool result = true;

	for (cont::iterator itr = tests_.begin(); itr != tests_.end(); ++itr)
	{
		if (!run_test(itr))
			result = false;
	}

	return result;
}

bool test_suite::run_test(cont::iterator itr)
{
	try
	{
		std::string name = itr->first;
		test* pTest = itr->second;
		std::cout << "Running test: " << name << std::endl;
		pTest->run();
		std::cout << "Passed" << std::endl;
		return true;
	}
	catch(failure&)
	{
		std::cout << "Failed" << std::endl;
		return false;
	}
}
