#include "pch.hpp"

#include <boost/rtl/generic.hpp>

#include <iostream>
#include "test_suite.hpp"

using namespace unit;

static bool test_app(int argc, char** argv)
{
	if (argc == 0)
		return test_suite::instance().run();

	else
	{
		while (argc-- != 0)
			test_suite::instance().run(*argv++);

		return true;
	}
}

int
main(int argc, char* argv[])
{
	if (!test_app(argc - 1, argv + 1))
	{
		std::cout << "*** Some tests failed!!! ***" << std::endl;
		return 1;
	}

	return 0;
}
