#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include "test.hpp"
#include "test_suite.hpp"
#include <stdexcept>

using namespace unit;

test::test(const std::string& name)
{
	bool result = test_suite::instance().add_test(name, *this);

	if (!result)
        throw std::runtime_error("can't add test");
}
