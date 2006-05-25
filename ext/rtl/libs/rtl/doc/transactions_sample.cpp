// transactions example

#include "table_delta.hpp"
#include "key_index_delta.hpp"
#include "range_selection_delta.hpp"
#include "transaction.hpp"
#include "utils.hpp"
#include "expression_registry.hpp"
#include <boost/date_time/gregorian/greg_date.hpp>

using namespace std;
using namespace boost;
using namespace gregorian;
using namespace rel;

// Since default constructor is not provided
// for gregorian::date, we need to define its default value

namespace rel
{
	template<>
		struct default_value<date>
	{
		static date get()
		{
			return date(1980, Jan, 1);
		}
	};
}

// operator<<() is only needed to be able to use the print() utility

ostream& operator<<(ostream& s, const date& d)
{
	s << d.month() << "/" << d.day()  << "/" << d.year();
	return s;
}

// define columns, table, and tuple types

BOOST_RTL_DEFINE_COLUMN(int, id);
BOOST_RTL_DEFINE_COLUMN(string, name);
BOOST_RTL_DEFINE_COLUMN(date, joined);

struct employee_info : table_info<
	mpl::vector<id, name, joined>, 
	mpl::vector<id> 
	> {};

typedef table<employee_info> employee_table;
typedef table<employee_info>::value_type employee_tuple;

// This relational expression is used to query 
// for all people joined between given dates,
// and sort them by name

typedef key_index_t<
	range_selection_t<
		key_index_t<employee_table, mpl::vector<joined, id> >::type, 
		lower_bound_t<row<mpl::vector<joined> > >, 
		lower_bound_t<row<mpl::vector<joined> > > 
		>::type, 
	mpl::vector<name, id> 
	>::type expr_type;

main()
{
	//allocate and populate the table (without using transactions)

	table<employee_info> t;

	t.insert(employee_tuple(1, "A", date(2003, Dec, 7)));
	t.insert(employee_tuple(2, "B", date(2003, Dec, 10)));
	t.insert(employee_tuple(3, "C", date(2003, Dec, 5)));
	t.insert(employee_tuple(4, "D", date(2003, Dec, 2)));
	t.insert(employee_tuple(5, "E", date(2003, Dec, 13)));
	t.insert(employee_tuple(6, "F", date(2003, Dec, 25)));
	t.insert(employee_tuple(7, "G", date(2003, Dec, 13)));
	t.insert(employee_tuple(8, "H", date(2003, Dec, 24)));
	t.insert(employee_tuple(9, "I", date(2003, Dec, 30)));
	t.insert(employee_tuple(10, "J", date(2003, Dec, 1)));
	
	// allocate the expression to query for people joined 
	// between 12/5/2003 and 12/25/2003, and sort them by name

	expr_type expr = key_index<mpl::vector<name, id> >(
		range_selection(
			key_index<mpl::vector<joined, id> >(t),
			lower_bound(row<mpl::vector<joined> >(date(2003, Dec, 5))), 
			lower_bound(row<mpl::vector<joined> >(date(2003, Dec, 25))) 
		)
	);

	// run the query

	print(expr);

	// allocate the transaction object

	transaction tr;
	
	// modify the table through the transaction

	tr.insert(t, employee_tuple(11, "K", date(2003, Dec, 7)));
	tr.insert(t, employee_tuple(12, "CC", date(2003, Dec, 17)));
	tr.remove(t, *t.lower_bound(row<mpl::vector<id> >(5)));

	// create the list of expressions to be updated at commit
	// in this example we have only one expression

	expression_registry exprs;
	exprs.add(expr);
	
	// commit the transaction.  Tell it to update the expression.
	// two indexes get incrementally updated rather than rebuilt.

	tr.commit(exprs);

	// reuse the query

	print(expr);

	return 0;
}
