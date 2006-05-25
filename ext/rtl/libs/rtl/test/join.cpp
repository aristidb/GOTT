#include "pch.hpp"

#include <string>
#include "test_function.hpp"
#include <boost/rtl/range_join.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/utils.hpp>

using namespace unit;
using namespace boost;
using namespace rel;
using namespace std;

namespace book
{
	BOOST_RTL_DEFINE_COLUMN(string, title);
	BOOST_RTL_DEFINE_COLUMN(int, author);

	typedef table<table_info<mpl::vector2<title, author> > > table_t;
	typedef table_t::value_type tuple_t;
}

namespace author
{
	BOOST_RTL_DEFINE_COLUMN(int, code);
	BOOST_RTL_DEFINE_COLUMN(string, name);

	typedef table<table_info<mpl::vector2<code, name> > > table_t;
	typedef table_t::value_type tuple_t;
}

namespace join_cpp
{
	void fill_tables(book::table_t& books, author::table_t& authors)
	{
		books.insert(book::tuple_t("Patterns", 1));
		books.insert(book::tuple_t("Refactoring", 2));
		books.insert(book::tuple_t("Essential COM", 3));

		authors.insert(author::tuple_t(1, "Gamma"));
		authors.insert(author::tuple_t(1, "Vlissides"));
		authors.insert(author::tuple_t(2, "Fawler"));
		authors.insert(author::tuple_t(3, "Box"));
	}

	void foo()
	{
		book::table_t books;
		author::table_t authors;
		fill_tables(books, authors);
		
		print(equal_join<mpl::vector1<book::author> >(books, authors));
		TESTASSERT(count(equal_join<mpl::vector1<book::author> >(books, authors)) == 4);

		equal_join_t<
			book::table_t, 
			author::table_t, 
			mpl::vector1<book::author> >::type j(books, authors);

		equal_join_t<
			book::table_t, 
			author::table_t, 
			mpl::vector1<book::author> >::type::const_iterator i; // default-constructible
	}

	test_function test("join", foo);
}
