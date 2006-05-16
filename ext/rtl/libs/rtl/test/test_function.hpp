#ifndef BOOST_RTL_TEST_FUNCTION_HPP_INCLUDED
#define BOOST_RTL_TEST_FUNCTION_HPP_INCLUDED

#include "test.hpp"

namespace unit
{
	class test_function : public test
	{
	public:
		test_function(const std::string& name, void(*pfn)()) : test(name), m_pfn(pfn) {}
		virtual void run()
		{
			(*m_pfn)();
		}

	private:
		void(*m_pfn)();
	};
}

#endif//BOOST_RTL_TEST_FUNCTION_HPP_INCLUDED
