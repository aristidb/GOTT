// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_UTILS_HPP_INCLUDED
#define BOOST_RTL_UTILS_HPP_INCLUDED

#include <boost/rtl/tuple.hpp>
#include <boost/rtl/iterator_root.hpp>
#include <iostream>
#include <functional>
#include <algorithm>
#include <boost/mpl/for_each.hpp>
//#include <typeinfo.h>

namespace boost { namespace rel {

	// print_field

	template <class Tuple>
		class print_field
	{
	public:
		print_field(const Tuple& t) : t_(t) {}
		template <class T> 
			void operator()(const T&)
		{
			std::cout << BOOST_RTL_FIELD(t_, T) << "\t";
		}

	private:
		const Tuple& t_;
	};

	// print_field

	//struct print_field_name
	//{
	//public:
	//	template <class T> 
	//		void operator()(T) const
	//	{
 //           std::cout << "\t" << typeid(T).name() << std::endl;
	//	}
	//};

 //   template<class T>
 //       void print_info(const T&)
 //   {
 //       std::cout << "SortList {" << std::endl;
 //       boost::mpl::for_each<typename T::sort_list>(print_field_name());
 //       std::cout << "}" << std::endl;
 //   };

    // print

	template <class Table>
	void print(const Table& tab)
	{
		typename Table::const_iterator e = tab.end(); 
		for (typename Table::const_iterator it = tab.begin(); it != e; ++it)
		{
			boost::mpl::for_each<
				typename Table::field_list>(print_field<typename Table::value_type>(*it));

			std::cout << std::endl;
		}

		std::cout << std::endl;
	}

	// count

	template <class Table>
	int count(const Table& tab)
	{
		int c = 0;
		typename Table::const_iterator e = tab.end();
		for (typename Table::const_iterator itr = tab.begin(); itr != e; ++itr)
			c++;
		
		return c;
	}

	// compare

	template <class Table1, class Table2>
	bool equal_tables(const Table1& t1, const Table2& t2)
	{
		BOOST_STATIC_ASSERT((list_equal<typename Table1::field_list, typename Table2::field_list>::value));

		typename Table1::const_iterator it1 = t1.begin();
		typename Table2::const_iterator it2 = t2.begin();

		typename Table1::const_iterator e1 = t1.end();
		typename Table2::const_iterator e2 = t2.end();

		while (it1 != e1 && it2 != e2)
		{
			if (compare_iterators<typename Table1::field_list>(it1, it2) != 0) //DA
				return false;

			++it1;
			++it2;
		}

		return it1 == e1 && it2 == e2;
	}
}}

#endif//BOOST_RTL_UTILS_HPP_INCLUDED
