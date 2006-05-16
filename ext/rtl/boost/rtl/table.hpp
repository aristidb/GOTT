// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
    
#ifndef BOOST_RTL_TABLE_HPP_INCLUDED
#define BOOST_RTL_TABLE_HPP_INCLUDED
    
#include <boost/rtl/tuple.hpp>
#include <boost/rtl/range.hpp>
#include <boost/rtl/iterator_root.hpp>
#include <boost/rtl/identifiable.hpp>
#include <boost/rtl/keys.hpp>
    
#ifdef BOOST_RTL_USE_SET
	#include <boost/rtl/indexed_set_impl.hpp>
    namespace boost { namespace rel {
		typedef indexed_set_strategy default_table_implementation;
    }}
#else
	#include <boost/rtl/vector_impl.hpp>
    namespace boost { namespace rel {
		typedef sorted_strategy default_table_implementation;
    }}
#endif

namespace boost { namespace rel {

	template<class Tuple, class SortList, class StrategyName>
		struct strategy : boost::mpl::apply2<StrategyName, Tuple, SortList>
	{};
    
	template<
        class FieldList, 
        class SortList = FieldList,
		class Keys = boost::mpl::vector1<SortList>,
		class Implementation = default_table_implementation
	> 
   		struct table_info
    {
    	typedef FieldList field_list;
    	typedef SortList sort_list;
		typedef Keys keys;
    	typedef Implementation implementation;
    };
    
	// table

	template<class Info> class table;
    
	template<class Info> class table_iterator : public iterator_root
	<
    	table_iterator<Info>,
    	table<Info> 
	>
   	{
		friend class iterator_root
 		<
    		table_iterator<Info>,
    		table<Info>
   		>;
   	public:
    	typedef typename table<Info>::data_iterator data_iterator;

    	typedef typename table_iterator::relation_type relation_type;
    	typedef typename table_iterator::value_type value_type;
    	typedef typename table_iterator::base_type base_type;

    public:
    	table_iterator(const data_iterator& i, const relation_type* o) 
    		: base_type(o)
    		, itr_(i)
    	{}
    	table_iterator() 
    		: itr_(data_iterator())
    	{}
    	bool operator== (const table_iterator& other) const
    	{
    		return 
    			itr_ == other.itr_ &&
    			this->relation_ == other.relation_;
    	}
    	
    	template<class Column>
    		typename Column::type
		get_internal(const Column& c) const 
    	{
    		return (*itr_)[c];
    	}
    
    	data_iterator base() const
    	{
    		return itr_;
    	}
    private:
    	void increment() 
    	{
    		++itr_;
    	}
    	void decrement() 
    	{
    		--itr_;
    	}
    private:
    	data_iterator itr_;
    };
    
    // MSVC ETI bug workaround

    template<class T> struct data_iterator
    {
        typedef typename T::data_iterator type;
    };

    #if defined(BOOST_MPL_MSVC_ETI_BUG)
    
    template<> struct data_iterator<int>
    {
        typedef int type;
    };
    #endif
    // end MSVC ETI bug workaround
    
    struct table_tag {};

    template <class Info> 
		class table : public identifiable
    {
    public:
        typedef table_tag tag;

		typedef typename Info::field_list field_list;
    
    	typedef row<field_list> value_type;
		
        typedef typename optimize_sort			
		<
			typename Info::sort_list,
			typename Info::keys
		>::type optimized_sort_list;

        /* AV: cover delta choke when sort_list on table and delta not same type,
           even if same sequence -- all this needs to be revisited!!! */

        typedef typename boost::mpl::if_<
            typename list_equal<optimized_sort_list, typename Info::sort_list>::type,
            typename Info::sort_list,
            optimized_sort_list
        >::type sort_list;

        //typedef typename Info::sort_list sort_list;

		typedef typename Info::keys keys;
    	typedef typename Info::implementation implementation;
    	typedef table_iterator<Info> const_iterator;
    	friend class table_iterator<Info>;

    private:
    	typedef typename strategy<value_type, sort_list, implementation>::type SortPolicy;
    	typedef typename data_iterator<SortPolicy>::type data_iterator;
		typedef std::pair<data_iterator, data_iterator> data_range;

    public:
		table() : data_ptr_(new SortPolicy) 
 		{}

        template<class R>
            void append(const R& r)
        {
            for (typename R::const_iterator it = r.begin(); it != r.end(); ++it)
                insert(*it);
        }

 		const_iterator begin() const {return const_iterator(data_ptr_->begin(),this);}
 		const_iterator end() const {return const_iterator(data_ptr_->end(),this);}

    	template <class SubTuple> 
    		range_t<const_iterator> equal_range(const SubTuple& sub) const
    	{
    		const bool valid = valid_subtuple<SubTuple,sort_list>::value;
    		BOOST_STATIC_ASSERT(valid);
            typedef typename rel::size<typename SubTuple::field_list>::type size_type;
			data_range p(data_ptr_->equal_range(sub, size_type()));
			range_t<const_iterator> r(const_iterator(p.first,this),const_iterator(p.second,this)); 
    		return r;
    	}

    	template<class SubTuple>
    		const_iterator lower_bound(const SubTuple& sub) const
    	{
    		const bool valid = valid_subtuple<SubTuple,sort_list>::value;
    		BOOST_STATIC_ASSERT(valid);
            typedef typename rel::size<typename SubTuple::field_list>::type size_type;
  			return const_iterator(data_ptr_->lower_bound(sub, size_type()), this);
    	}

    	template<class SubTuple>
    		const_iterator upper_bound(const SubTuple& sub) const
    	{
    		const bool valid = valid_subtuple<SubTuple,sort_list>::value;
    		BOOST_STATIC_ASSERT(valid);
            typedef typename rel::size<typename SubTuple::field_list>::type size_type;
  			return const_iterator(data_ptr_->upper_bound(sub, size_type()), this);
    	}

  		void insert(const value_type& t) 
  		{
  			data_ptr_->insert(t);
  		}

    	void remove(const const_iterator& i) 
    	{
			data_ptr_->remove(i.base());
   		}

		void clear()
		{
			data_ptr_->clear();
		}

   		template<class SubTuple>
    		void update(const const_iterator& i, const SubTuple& sub)
    	{
    		value_type tup;
    		copy_fields<typename SubTuple::field_list>(sub, tup);
 			data_ptr_->update(i.base(),tup);
  		}

 		void reserve(std::size_t new_size) 
 		{
 			data_ptr_->reserve(new_size);
 		}

 		std::size_t size() const 
 		{
 			return data_ptr_->size();
 		}

    	template<class F>
    		void traverse_args(F f) const
    	{}
    
   	private:
 		boost::shared_ptr<SortPolicy> data_ptr_;
   	};

    template<class R>
        struct table_type
    {
        typedef table<table_info<
            typename R::field_list,
            typename R::sort_list,
            typename R::keys
            > > type;
    };
}}
    
#endif//BOOST_RTL_TABLE_HPP_INCLUDED


