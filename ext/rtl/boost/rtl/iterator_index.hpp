// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_ITERATOR_INDEX_HPP_INCLUDED
#define BOOST_RTL_ITERATOR_INDEX_HPP_INCLUDED
    
#include <boost/rtl/index.hpp>
#include <boost/rtl/list.hpp>
    
namespace boost { namespace rel {

	template<class Table, class SortList>
    	struct itx_
    {
  		typedef row
 		<
 			typename concat<SortList,boost::mpl::vector1<idxr<Table> > >::type
    	> type;
    };
    
    template<class Table, class SortList, class Strategy> class it_idx_t;
    
    template<class Table, class SortList, class Strategy> 
		class iterator_index_iterator : public index_iterator
		<
    		iterator_index_iterator<Table, SortList, Strategy>, 
    		it_idx_t<Table, SortList, Strategy> 
		>
    {
    	typedef index_iterator
			<
                iterator_index_iterator<Table, SortList, Strategy>, 
                it_idx_t<Table, SortList, Strategy> 
    		> base_type;
    
    	friend class index
			<
    			Table,
    			SortList,
    			typename itx_<Table,SortList>::type,
    			Strategy,
    			iterator_index_iterator<Table,SortList,Strategy>,
    			it_idx_t<Table,SortList,Strategy> 
			>;
    
    public:
    	BOOST_RTL_PULL_ITERATOR_TYPEDEFS(base_type);
    	typedef typename base_type::idx_iterator idx_iterator;
  		using base_type::itr_;
        typedef typename Table::const_iterator tab_iterator;
    	
    private:
    	iterator_index_iterator(const idx_iterator& itr, const relation_type* o) 
			: base_type(itr, o) 
		{}

  		const idx_iterator& getIndexIterator() const {return itr_;}

    public:
    	iterator_index_iterator() {};

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
    
    struct iterator_index_tag {};
  	
    template<class Table, class SortList, class Strategy>
 		class it_idx_t  : public index
  			<
    				Table,
    				SortList,
    				typename itx_<Table,SortList>::type,
    				Strategy,
    				iterator_index_iterator<Table,SortList,Strategy>,
    				it_idx_t<Table,SortList,Strategy>
 			> 
    {
    private:
		typedef index
  		<
    	    Table,
    	    SortList,
    	    typename itx_<Table,SortList>::type,
    	    Strategy,
    	    iterator_index_iterator<Table,SortList,Strategy>,
    	    it_idx_t<Table,SortList,Strategy>
 		> base_type;

  		typedef idxr<Table> numerator_type;
    	friend class iterator_index_iterator<Table, SortList,Strategy>;
    public:
        typedef iterator_index_tag tag;

  		BOOST_RTL_PULL_INDEX_TYPEDEFS(base_type);
		typedef typename Table::const_iterator tab_iterator;
        using base_type::table_;
        using base_type::index_;

    	const Table& getTable() const {return *table_;}

  		it_idx_t(const Table& t) : base_type(t)
  		{
    		initialize();
		}	

    	void refill()
    	{
 			index_.clear();
    		initialize();
    	}

  		void insert_iterator(const tab_iterator& itr)
  		{
  			index_tuple temp;
            copy_fields<SortList>(itr, temp);
  			BOOST_RTL_FIELD(temp, numerator_type) = itr;
  			index_.insert(temp);
  		}

	private:
		void initialize()
    	{
    		// m_index.reserve(...); sometimes it is easy to calulate
  			tab_iterator e = table_->end();
  			for(tab_iterator itr = table_->begin(); itr != e; ++itr)
  			{
  				insert_iterator(itr);
  			}
    	}
    };
    
    template<class Table, class SortList, class Strategy = default_table_implementation> 
    	struct iterator_index_t
    {
    	typedef it_idx_t<Table, SortList, Strategy> type;
    };
    
    template<class SortList, class Table>
    	typename iterator_index_t<Table, SortList>::type 
    iterator_index(const Table& t, const SortList* s = 0)
    {
    	return typename iterator_index_t<Table,SortList>::type(t);
    }
}}
    
#endif//BOOST_RTL_ITERATOR_INDEX_HPP_INCLUDED
