#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include <iostream>
#include <set>
#include <vector>
#include <utility>
#include <boost/smart_ptr.hpp>
#include <boost/rtl/utils.hpp>
#include "test_function.hpp"
#include <boost/rtl/table_delta.hpp> 
#include <boost/rtl/key_index_delta.hpp>
#include <boost/rtl/iterator_index_delta.hpp>
#include <boost/rtl/rename_delta.hpp>
#include <boost/rtl/crossproduct_delta.hpp>
#include <boost/rtl/selection_delta.hpp>
#include <boost/rtl/select_functors.hpp>
#include <boost/rtl/projection_delta.hpp>
#include <boost/rtl/cover_delta.hpp>
#include <boost/rtl/merge_delta.hpp>
#include <boost/rtl/zigzag_delta.hpp>
#include <boost/rtl/range_selection_delta.hpp>
#include <boost/rtl/groupby_delta.hpp>
#include <boost/rtl/expression_registry.hpp>
#include <boost/function.hpp>

using namespace unit;
using namespace boost;
using namespace rel;
using namespace std;
using namespace lambda;

namespace tt
{
	template<class Base> 
	class trans_test : public Base
	{
		typedef typename Base::relation_type relation_type;
	public:
		virtual void run()
		{
			relation_type r_before = this->create_relation();
			expression_registry exprs;
			exprs.add(r_before);

			transaction tr;
			this->modify_tables(tr);
			
			table<table_info<typename Base::relation_type::field_list, typename Base::relation_type::sort_list> > r_modified;
			typename traits<relation_type>::modified_type m = modified(r_before, tr);

			for (typename traits<relation_type>::modified_type::const_iterator it =
				m.begin(); it != m.end(); ++it)
				r_modified.insert(*it);

			tr.commit(exprs);
			relation_type r_after = this->create_relation();

			print(r_before);
			print(r_after);

			TESTASSERT(equal_tables(r_before, r_after));
			TESTASSERT(equal_tables(r_modified, r_after));
		}
	};

	class unary_test : public unit::test
	{
	protected:
		BOOST_RTL_DEFINE_COLUMN(int, c1);
		BOOST_RTL_DEFINE_COLUMN(int, c2);
		typedef table<table_info<mpl::vector2<c1, c2> > > table_type;
		typedef table_type::value_type tuple_type;
		table_type t;

		unary_test(const char* name)
			: unit::test(name)
		{
			for (int i = 0; i < 5; ++i)
				t.insert(tuple_type(i, 5 - i));
		}
		void modify_tables(transaction& tr)
		{
			TESTASSERT(tr.insert(t, tuple_type(5, 0)));
			TESTASSERT(tr.remove(t, tuple_type(1, 4)));
		}
	};

	class binary_test : public unit::test
	{
	protected:
		BOOST_RTL_DEFINE_COLUMN(int, c1);
		typedef table<table_info<mpl::vector1<c1> > > table_type;
		typedef table_type::value_type tuple_type;
		table_type t1;
		table_type t2;

		binary_test(const char* name)
			: unit::test(name)
		{
			for (int i = 0; i < 5; ++i)
			{
				t1.insert(tuple_type(i));
				t2.insert(tuple_type(i + 2));
			}
		}
		void modify_tables(transaction& tr)
		{
   			TESTASSERT(tr.insert(t1, tuple_type(5)));
			TESTASSERT(tr.remove(t1, tuple_type(3)));
			TESTASSERT(tr.insert(t2, tuple_type(0)));
			TESTASSERT(tr.remove(t2, tuple_type(4)));
		}
	};

	class key_index_test : public unary_test
	{
	protected:
		typedef key_index_t<table_type, mpl::vector2<c2, c1> >::type relation_type;

		key_index_test()
			: unary_test("key_index_test")
		{}
		relation_type create_relation()
		{
			return relation_type(t);
		}
	};

	trans_test<key_index_test> _key_index_test; 

	class iter_index_test : public unary_test
	{
	protected:
		typedef iterator_index_t<table_type, mpl::vector2<c2, c1> >::type relation_type;

		iter_index_test()
			: unary_test("iter_index_test")
		{}
		relation_type create_relation()
		{
			relation_type idx = relation_type(t);
			indexes_.push_back(idx);
			return idx;
		}
	private:
		vector<relation_type> indexes_;
	};

	trans_test<iter_index_test> _iter_index_test; 

	class auto_rename_test : public unary_test
	{
	protected:
		typedef auto_rename_t<table_type, void>::type relation_type;

		relation_type create_relation() const
		{
			return relation_type(t);
		}
		auto_rename_test()
			: unary_test("auto_rename_test")
		{}
	};

	trans_test<auto_rename_test> _auto_rename_test; 

/*	class x_product_test : public binary_test
	{
	protected:
		typedef cross_product_t<table_type, auto_rename_t<table_type, void>::type>::type relation_type;

		relation_type create_relation() const
		{
			return cross_product(t1, auto_rename<void>(t2));
		}
		x_product_test()
			: binary_test("x_product_test")
		{}
	};

	trans_test<x_product_test> _x_product_test; 
*/
    class selection_test : public unary_test
	{
        struct pred
        {
            template<class It>
                bool operator()(const It& it) const
            {
                return it[c1()]<3 || it[c2()]<3;
            }
        };
	protected:
		typedef selection_t<table_type, pred>::type relation_type;

		relation_type create_relation() const
		{
            return selection(t, pred());
		}
		selection_test()
			: unary_test("selection_test")
		{}
	};

	trans_test<selection_test> _selection_test;
	class projection_test : public unary_test
	{
	protected:
		typedef projection_t<table_type, mpl::vector2<c2, c1> >::type relation_type;

		relation_type create_relation() const
		{
			return projection<mpl::vector2<c2, c1> >(t);
		}
		projection_test()
			: unary_test("projection_delta_test")
		{}
	};

	trans_test<projection_test> _projection_test; 

	class selection_ge_test : public unary_test
	{
	protected:
		typedef selection_ge_t<table_type, row<mpl::vector1<c1> > >::type relation_type;

		relation_type create_relation() const
		{
			return selection_ge(t, row<mpl::vector1<c1> >(1));
		}
		selection_ge_test()
			: unary_test("selection_ge_test")
		{}
	};

	trans_test<selection_ge_test> _selection_ge_test; 

	class selection_gt_test : public unary_test
	{
	protected:
		typedef selection_gt_t<table_type, row<mpl::vector1<c1> > >::type relation_type;

		relation_type create_relation() const
		{
			return selection_gt(t, row<mpl::vector1<c1> >(1));
		}
		selection_gt_test()
			: unary_test("selection_gt_test")
		{}
	};

	trans_test<selection_gt_test> _selection_gt_test; 

	class selection_le_test : public unary_test
	{
	protected:
		typedef selection_le_t<table_type, row<mpl::vector1<c1> > >::type relation_type;

		relation_type create_relation() const
		{
			return selection_le(t, row<mpl::vector1<c1> >(1));
		}
		selection_le_test()
			: unary_test("selection_le_test")
		{}
	};

	trans_test<selection_le_test> _selection_le_test; 

	class selection_lt_test : public unary_test
	{
	protected:
		typedef selection_lt_t<table_type, row<mpl::vector1<c1> > >::type relation_type;

		relation_type create_relation() const
		{
			return selection_lt(t, row<mpl::vector1<c1> >(1));
		}
		selection_lt_test()
			: unary_test("selection_lt_test")
		{}
	};

	trans_test<selection_lt_test> _selection_lt_test; 

	class selection_eq_test : public unary_test
	{
	protected:
		typedef selection_eq_t<table_type, row<mpl::vector1<c1> > >::type relation_type;

		relation_type create_relation() const
		{
			return selection_eq(t, row<mpl::vector1<c1> >(1));
		}
		selection_eq_test()
			: unary_test("selection_eq_test")
		{}
	};

	trans_test<selection_eq_test> _selection_eq_test; 
}
