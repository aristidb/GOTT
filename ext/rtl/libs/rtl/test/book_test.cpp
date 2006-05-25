#include "pch.hpp"

#pragma message("started")

#include <string>
#include <iostream>

#include <boost/rtl/generic.hpp>
#include <boost/rtl/table.hpp>

#pragma message("included table.hpp")

#include "test_function.hpp"
#include <boost/rtl/utils.hpp>
#include <boost/rtl/selection.hpp>
#include <boost/rtl/projection.hpp>
#include <boost/rtl/crossproduct.hpp>
#include <boost/rtl/select_functors.hpp>

namespace book_test_cpp
{
	using namespace unit;
	using namespace std;
	using namespace boost;
	using namespace rel;
	using namespace lambda;

	namespace book
	{
		BOOST_RTL_DEFINE_COLUMN(string, title);
		BOOST_RTL_DEFINE_COLUMN(int, author);

		typedef table<table_info<
			mpl::vector2<title, author> 
			> > table_t;
		
		typedef table_t::value_type tuple_t;
	}

	namespace author
	{
		BOOST_RTL_DEFINE_COLUMN(int, code);
		BOOST_RTL_DEFINE_COLUMN(string, name);

		typedef table<table_info<
			mpl::vector2<code, name> 
			> > table_t;
		
		typedef table_t::value_type tuple_t;
	}

	template <class Table> void prettyprint(const Table& t)
	{
		for (typename Table::const_iterator it = t.begin(); it != t.end(); ++it)
		{
			typename Table::value_type tup = *it;
			std::cout 
				<< BOOST_RTL_FIELD(tup, book::title) << BOOST_RTL_FIELD(tup, author::name) << std::endl; 
		}
	}
	void foo()
	{
		author::table_t authors;
		authors.insert(author::tuple_t(1, "Bjarne Stroustrup"));
		authors.insert(author::tuple_t(2, "Andrei Alexandrescu"));
		authors.insert(author::tuple_t(3, "Don Box"));
		authors.insert(author::tuple_t(4, "Martin Fawler"));

		book::table_t books;
		books.insert(book::tuple_t("The C++ Programming Language", 1));
		books.insert(book::tuple_t("Modern C++ Design", 2));
		books.insert(book::tuple_t("Essential COM", 3));
		books.insert(book::tuple_t("Effective COM", 3));
		books.insert(book::tuple_t("Analysis Patterns", 4));
		books.insert(book::tuple_t("Refactoring", 4));

#pragma message("started expression")

        print(projection<mpl::vector3<author::code, author::name, book::title> >(
			selection(
				cross_product(authors, books), 
				lambda::_1[author::code()] == lambda::_1[book::author()]
				)));
		
#pragma message("done expression1")

        // this is not recommended

		typedef	
			projection_t<
				selection_t<
					cross_product_t<
						author::table_t, 
						book::table_t 
						>::type, 
					eq2<author::code, book::author> 
					>::type,
				mpl::vector3<author::code, author::name, book::title> 
				>::type myjoin_t;
		
		myjoin_t join = projection<mpl::vector3<author::code, author::name, book::title> >(
			selection(
				cross_product(authors, books), 
				eq2<author::code, book::author>()
				));

#pragma message("done expression2")

		for (myjoin_t::const_iterator it = join.begin(); it != join.end(); ++it)
		{
			myjoin_t::value_type tup = *it;
			cout << BOOST_RTL_FIELD(tup, book::title) << BOOST_RTL_FIELD(tup, author::name) << endl; 
		}
		// this is recommended

		prettyprint(
			projection<mpl::vector3<author::code, author::name, book::title> >(
				selection(
					cross_product(authors, books), 
					lambda::_1[author::code()] == lambda::_1[book::author()]
					)));
	}

	test_function test("book_test", foo);
}
#pragma message("really done")
