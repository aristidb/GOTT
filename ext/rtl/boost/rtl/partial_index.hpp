// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_PARTIAL_INDEX_HPP_INCLUDED
#define BOOST_RTL_PARTIAL_INDEX_HPP_INCLUDED

#include <boost/rtl/index.hpp>

// this index was designed for using internally
namespace boost { namespace rel {

	template<class Table>
    	struct px_
    {
  		typedef row
 		<
			typename concat<typename Table::sort_list,boost::mpl::vector1<idxr<Table> > >::type
    	> type;
    };


	template<class Table, class Strategy> class ix_t;

	template<class Table, class Strategy> 
		class partial_index_iterator : public index_iterator
		<
			partial_index_iterator<Table,Strategy>,
			ix_t<Table,Strategy>
		>
	{
    	typedef index_iterator
		<
			partial_index_iterator<Table,Strategy>, 
			ix_t<Table,Strategy> 
		> base_type;

		friend class index
		<
			Table,				
			typename Table::sort_list,                        
			typename px_<Table>::type,		
			Strategy,
			partial_index_iterator<Table,Strategy>,
			ix_t<Table,Strategy>
		>;
	public:
    	BOOST_RTL_PULL_ITERATOR_TYPEDEFS(base_type);
        using base_type::itr_;

  		typedef typename Table::const_iterator tab_iterator;
    private:
        partial_index_iterator(const typename base_type::idx_iterator& itr, const relation_type* o) 
			: base_type(itr, o) 
		{}

  		const typename base_type::idx_iterator& getIndexIterator() const {return itr_;}
    public:
    	partial_index_iterator() {};

		tab_iterator get_iterator() const
		{
			typename relation_type::numerator_type num;
 			return itr_.get(num);
 		}

    	template<class Column>
    		typename Column::type
		get_internal(const Column& c) const 
    	{
  			tab_iterator i = get_iterator();
  			return i.get(c);
    	}
	};

	template<class Table, class Strategy> 
		struct ix_base : boost::mpl::identity
		<
			index
			<
				Table,				
				typename Table::sort_list,
				typename px_<Table>::type,		
				Strategy,
				partial_index_iterator<Table,Strategy>,
				ix_t<Table,Strategy>
			>
		>
	{};

    struct partial_index_tag {};

	template<class Table, class Strategy>
		class ix_t : public ix_base<Table,Strategy>::type
	{
    private:
		typedef typename ix_base<Table,Strategy>::type base_type; 
  		typedef idxr<Table> numerator_type;
    	friend class partial_index_iterator<Table,Strategy>;
    public:
        typedef partial_index_tag tag;

  		BOOST_RTL_PULL_INDEX_TYPEDEFS(base_type);
        using base_type::index_;
        using base_type::table_;

		typedef typename Table::const_iterator tab_iterator;

    	const Table& getTable() const {return *table_;}

  		ix_t(const Table& t) : base_type(t)
  		{
		}	

  		void insert_iterator(const tab_iterator& itr)
  		{
  			index_tuple temp;
  			copy_fields<typename Table::sort_list>(itr, temp);
  			BOOST_RTL_FIELD(temp, numerator_type) = itr;
  			index_.insert(temp);
  		}
	};

    template<class Table,class Strategy = default_table_implementation> 
    	struct partial_index_t
    {
    	typedef ix_t<Table,Strategy> type;
    };

    template<class Table>
    	typename partial_index_t<Table>::type 
    partial_index(const Table& t)
    {
    	return typename partial_index_t<Table>::type(t);
    }
}}

#endif

