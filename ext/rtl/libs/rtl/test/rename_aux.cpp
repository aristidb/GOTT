#include "pch.hpp"

//#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <boost/rtl/generic.hpp>
#include "test_function.hpp"
#include <boost/rtl/rename.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/utils.hpp>
#include "test_tables.hpp"
#include <boost/rtl/crossproduct.hpp>

namespace rename_cpp
{
	using namespace boost;
	using namespace rel;
	using namespace unit;

	BOOST_RTL_DEFINE_COLUMN(int, ssn1);
	BOOST_RTL_DEFINE_COLUMN(std::string, name1);
	BOOST_RTL_DEFINE_COLUMN(int, year1);
	BOOST_RTL_DEFINE_COLUMN(std::string, project1);
	BOOST_RTL_DEFINE_COLUMN(int, country1);

	typedef boost::mpl::vector5<ssn1,name1,year1,project1,country1> vec;

	static void test5()
	{
		print
		(
			cross_product
			(
				staff::table(), 
				manual_rename<vec>(staff::table())
			)
		);
	}
	
	test_function t5("Rename5", test5);
}
