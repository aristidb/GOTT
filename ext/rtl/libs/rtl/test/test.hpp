#ifndef BOOST_RTL_TEST_HPP_INCLUDED
#define BOOST_RTL_TEST_HPP_INCLUDED

#include <string>
#include <iostream>

/*
#define TESTASSERT(b)\
	if (!(b)){\
		std::cout << __FILE__ << "(" << __LINE__ << ") : " << #b << std::endl;\
		throw unit::failure();\
	}
*/

#define TESTASSERT(b)\
	if (!(b)){\
		std::cout << "Failed: " << #b << std::endl;\
		throw unit::failure();\
	}

namespace unit
{
	struct failure {};

	class test
	{
	public:
		virtual ~test(){}
		explicit test(const std::string& name);
		virtual void run() = 0;
	};
}

#endif//BOOST_RTL_TEST_HPP_INCLUDED
