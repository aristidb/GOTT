#ifndef BOOST_RTL_TEST_TABLES_HPP_INCLUDED
#define BOOST_RTL_TEST_TABLES_HPP_INCLUDED

#include <boost/rtl/tuple.hpp>
#include <boost/rtl/table.hpp>
#include <string>
#include <memory>
#include <iostream>
#include <boost/mpl/vector.hpp>

template<class T>
class OrdinaryPtr
{
public:
	OrdinaryPtr() : m_p(0) {}
	OrdinaryPtr(const T* p) : m_p(p) {}
	OrdinaryPtr(const OrdinaryPtr<T>& sp) : m_p(sp.getPtr()) {}
	~OrdinaryPtr() {};
	bool operator <(const OrdinaryPtr<T>& sp) const
	{
		return(*getPtr() < *sp.getPtr());
	}
	bool operator ==(const OrdinaryPtr<T>& sp) const
	{
		return(*getPtr() == *sp.getPtr());
	}
	const T* getPtr() const 
	{
		return m_p;
	}
	
private:
	const T* m_p;
};

template<class T>
std::ostream& operator << ( std::ostream& os, const OrdinaryPtr<T>& p )
{
		os << *(p.getPtr());   
		return os;
}

namespace heapstr
{
	typedef OrdinaryPtr<std::string> StringPtr;
	typedef std::auto_ptr<std::string> StringOwner;
	static StringOwner d1(new std::string("first_dept"));	
	static StringPtr dept1(d1.get());
	static StringOwner d2(new std::string("second_dept"));	
	static StringPtr dept2(d2.get());
	static StringOwner n1(new std::string("first_name"));	
	static StringPtr name1(n1.get());
	static StringOwner n2(new std::string("second_name"));	
	static StringPtr name2(n2.get());
	static StringOwner n3(new std::string("second_name"));	
	static StringPtr name3(n3.get());

}


namespace depts
{
	BOOST_RTL_DEFINE_COLUMN(heapstr::StringPtr, dept);
	BOOST_RTL_DEFINE_COLUMN(heapstr::StringPtr, name);
	
	struct table : public boost::rel::table<boost::rel::table_info<
		boost::mpl::vector2<name, dept>
		> >
	{
		table()
		{
			insert(value_type(heapstr::name1,heapstr::dept1));
			insert(value_type(heapstr::name2,heapstr::dept1));
			insert(value_type(heapstr::name1,heapstr::dept2));
		}
	};

	typedef table::value_type tuple_type;
}

namespace people
{
	BOOST_RTL_DEFINE_COLUMN(heapstr::StringPtr, name);
	BOOST_RTL_DEFINE_COLUMN(int, salary);

	struct table : public boost::rel::table<boost::rel::table_info<
		boost::mpl::vector2<name, salary>
		> >
	{
		table()
		{
			insert(value_type(heapstr::name1,100));
			insert(value_type(heapstr::name3,200));
		}
	};

	typedef table::value_type tuple_type;
}

namespace staff
{
	BOOST_RTL_DEFINE_COLUMN(int, ssn);
	BOOST_RTL_DEFINE_COLUMN(std::string, name);
	BOOST_RTL_DEFINE_COLUMN(int, year);
	BOOST_RTL_DEFINE_COLUMN(std::string, project);
	BOOST_RTL_DEFINE_COLUMN(int, country);

	struct table : public boost::rel::table<boost::rel::table_info<
		boost::mpl::vector5<ssn, name, year, project, country> 
		> >
	{
		table()
		{
			insert(value_type(1, "Vitaliy", 1966, "Dialer", 4));
			insert(value_type(2, "Tomas", 1970, "Loader", 2));
			insert(value_type(3, "Guy", 1972, "Phoenix", 3));
		}
	};

	typedef table::value_type tuple_type;
}

namespace account
{
	BOOST_RTL_DEFINE_COLUMN(int,sn);
	BOOST_RTL_DEFINE_COLUMN(std::string,zip);

    typedef boost::mpl::vector2<sn, zip> lst;

	struct table : public boost::rel::table<boost::rel::table_info<
		boost::mpl::vector2<sn, zip>
		> >
	{
		table()
		{
			value_type tp;
			BOOST_RTL_FIELD(tp, sn) = 1;
			BOOST_RTL_FIELD(tp, zip) = "zip11";
			insert(tp);

			BOOST_RTL_FIELD(tp, sn) = 1;
			BOOST_RTL_FIELD(tp, zip) = "zip12";
			insert(tp);

			BOOST_RTL_FIELD(tp, sn) = 2;
			BOOST_RTL_FIELD(tp, zip) = "zip21";
			insert(tp);
		}
	};

	typedef table::value_type tuple_type;
}

namespace myspace
{
	BOOST_RTL_DEFINE_COLUMN(int,sn)
	BOOST_RTL_DEFINE_COLUMN(std::string,qq)
	
	struct table : public boost::rel::table<boost::rel::table_info<
		boost::mpl::vector2<sn, qq>
		> >
	{
		table()
		{
			value_type tp;

			BOOST_RTL_FIELD(tp, sn) = 1;
			BOOST_RTL_FIELD(tp, qq) = "zip11";
			insert(tp);

			BOOST_RTL_FIELD(tp, sn) = 1;
			BOOST_RTL_FIELD(tp, qq) = "zip21";
			insert(tp);

			BOOST_RTL_FIELD(tp, sn) = 2;
			BOOST_RTL_FIELD(tp, qq) = "zip21";
			insert(tp);

			BOOST_RTL_FIELD(tp, sn) = 0;
			BOOST_RTL_FIELD(tp, qq) = "zip31";
			insert(tp);
		}
	};

	typedef table::value_type tuple_type;
}

namespace country
{
	BOOST_RTL_DEFINE_COLUMN(short, code);
	BOOST_RTL_DEFINE_COLUMN(std::string, name);

	struct table : public boost::rel::table<boost::rel::table_info<
		boost::mpl::vector2<code, name>
		> >
	{
		table()
		{
			insert(value_type(1, "U.S.A."));
			insert(value_type(2, "Ireland"));
			insert(value_type(3, "Israel"));
			insert(value_type(4, "Russia"));
		}
	};

	typedef table::value_type tuple_type;
}


#endif//BOOST_RTL_TEST_TABLES_HPP_INCLUDED
