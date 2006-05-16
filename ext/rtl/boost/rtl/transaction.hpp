// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_TRANSACTION_HPP_INCLUDED
#define BOOST_RTL_TRANSACTION_HPP_INCLUDED

#include <set>
#include <vector>
#include <algorithm>
#include <boost/mem_fn.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/rtl/tuple.hpp>
#include <boost/rtl/range.hpp>
#include <boost/rtl/identifiable.hpp>
#include <boost/rtl/iterator_index.hpp>
#include <boost/rtl/utils.hpp>
#include <boost/rtl/partial_index.hpp>
#include <boost/rtl/partial_index_delta.hpp>

/////////////////////////////////////////////////////////////////////////////////
//                           Table deltas
//
// For any tuple provided it may happen exactly one of following pre-conditions:
//
//		1) same-key tuple exists in the base Table, in Minus, and in Plus;  {T,M,P}
//		2) same-key tuple exists is in the base Table and in Minus;         {T,M}
//		3) same-key tuple exists is in the base Table only;                 {T}
//		4) same-key tuple exists is in Plus only;                           {P}
//		5) same-key tuple exists is absent everywhere;                      {}
//
// Let us consider operation REMOVE with the tuple as parameter:
// 
// Pre-condition Action                 Result
// ============= ======                 ======
// {T,M,P}       P.remove(key(tuple))   true
// {T,M}         none                   false
// {T}           M.add(tuple)           true
// {P}           P.remove(key(tuple))   true
// {}            none                   false
//
// Let us consider operation INSERT with the tuple as parameter:
//
// Pre-condition    Action          Result
// =============    ======          ======
// {T,M,P}          none            false
// {T,M}            P.insert(tuple) true
// {T}              none            false
// {P}              none            false
// {}               P.insert(tuple) true
//
// Let us consider operation UPDATE with the tuple as parameter:
//
///////////////////////////////////////////////////////////////////////////////


namespace boost { namespace rel {

	template<class R, class Sub> 
		bool find(const R& tab, const Sub& sub, typename R::const_iterator& it) 
	{
		BOOST_STATIC_ASSERT((boost::is_same<
			typename R::sort_list, 
			typename Sub::field_list>::value));

		range_t<typename R::const_iterator> r(tab.equal_range(sub));

		if (r.begin() == r.end()) 
			return false;

		it = r.begin().base();
		return true;
	}

	template<class R, class Sub>
		bool tuple_exists(const R& tab, const Sub& sub) 
	{
		BOOST_STATIC_ASSERT((boost::is_same<
			typename R::sort_list, 
			typename Sub::field_list>::value));

		range_t<typename R::const_iterator> r(tab.equal_range(sub));
		return (r.begin() != r.end());
	}

	template<class Table>
		struct table_delta
	{
		typedef typename Table::sort_list table_sort_list;
		typedef Table plus_type; 	
		typedef ix_t<Table, typename Table::implementation> minus_type;
		typedef std::pair<plus_type, minus_type> delta_type;
	};

	class transaction;

	struct table_proxy
	{
		virtual ~table_proxy(){}
		virtual ptrdiff_t id() const = 0;
		virtual void commit() = 0;
	};

	template<class Table>
		class table_proxy_impl : public table_proxy
	{
		friend class transaction;

	public:
		typedef typename table_delta<Table>::minus_type minus_type;
		typedef typename minus_type::const_iterator minus_iterator;
		typedef typename table_delta<Table>::plus_type plus_type;
		typedef typename plus_type::const_iterator plus_iterator;
		typedef row<typename Table::sort_list> sort_tuple;
		typedef std::vector<bool> update_flags;

		virtual ptrdiff_t id() const
		{
			return table_.id();
		}

		void process_updates(update_flags& m_flags, update_flags& p_flags)
		{
			minus_iterator mi = delta_.second.begin();
			const minus_iterator me = delta_.second.end();
			update_flags::iterator mfi = m_flags.begin();

			plus_iterator pi = delta_.first.begin();
			const plus_iterator pe = delta_.first.end();
			update_flags::iterator pfi = p_flags.begin();			
			while ((mi != me) && (pi != pe))
			{
				int result = compare_iterators<typename Table::sort_list>(mi,pi);
				if(result==0)
				{
					typename Table::const_iterator it = mi.get_iterator();
					const typename Table::value_type& tup = *pi;
					table_.update(it,tup);
					*mfi = true;
					*pfi = true;
				}
				if(result<=0)
				{
					++mfi;
					++mi;
				}
				if(result>=0)
				{
					++pfi;
					++pi;
				}
			}
		}

		void process_removes(const update_flags& m_flags)
		{
			const minus_iterator e = delta_.second.end();
			update_flags::const_iterator mfi = m_flags.begin();
			for (minus_iterator i = delta_.second.begin(); i != e; ++mfi,++i)
			{
				if (!(*mfi))
				{
					typename Table::const_iterator it = i.get_iterator();
					table_.remove(it);
				}
			}
		}

		void process_inserts(const update_flags& p_flags)
		{
			plus_iterator e = delta_.first.end();
			update_flags::const_iterator pfi = p_flags.begin();
			for (plus_iterator i = delta_.first.begin(); i != e; ++pfi,++i)
			{
				if(!(*pfi))
				{
					table_.insert(*i);
				}
			}
		}

		virtual void commit()
		{
			std::vector<bool> m_flags(delta_.second.size(),false);
			std::vector<bool> p_flags(delta_.first.size(),false);
			process_updates(m_flags,p_flags);
			process_removes(m_flags);
			process_inserts(p_flags);
		}
	public:
		table_proxy_impl(const Table& table)
			: table_(table)
			, delta_(std::make_pair(plus_type(), minus_type(table)))
		{}

		bool insert(const typename Table::value_type& tuple)
		{
			sort_tuple sub;
			copy_fields<typename Table::sort_list>(tuple, sub);

			if (tuple_exists(delta_.first, sub))
			{
				return false;
			}

			if (tuple_exists(delta_.second,sub))
			{
				delta_.first.insert(tuple);
				return true;
			}

			if (tuple_exists(table_, sub))
			{
				return false;
			}
			delta_.first.insert(tuple);
			return true;
		}

		bool remove(const typename Table::value_type& tup)
		{
			sort_tuple sub;
			copy_fields<typename Table::sort_list>(tup, sub);

			if (remove_from_plus(sub))
				return true;

			if (tuple_exists(delta_.second, sub)) 
				return false;

			typename Table::const_iterator tit;
            if (!rel::find(table_, sub, tit))
				return false;

			delta_.second.insert_iterator(tit);
			return true;
		}

		bool remove(const typename Table::const_iterator& it)
		{
			sort_tuple sub;
			copy_fields<typename Table::sort_list>(*it, sub);  // TODO!!!

			if (remove_from_plus(sub))
				return true;

			if (tuple_exists(delta_.second, sub)) 
				return false;

			delta_.second.insert_iterator(it);
			return true;
		}
		
		/*
		The following two update functions could be optimized
		for the case when the tuple being modified has already
		been added or updated, and the sort_list part of it
		is not being changed.  In this case it would be possible to
		perform in-place modification in plus.

		I don't consider this optimization necessary since:
		
		1) deltas are normally small -- it's cheap to remove/add
		2) the situation described should be rather rare.

		If you still want to optimize this, please justify!  AV
		*/
		
		bool update(
			const typename Table::const_iterator& it, 
			const typename Table::value_type& tup)
		{
			if (!remove(it))
				return false;

			if (compare_tuples<typename Table::sort_list>(*it, tup) == 0)
			{
				delta_.first.insert(tup);
				return true;
			}
			else
			{
				if (insert(tup))
					return true;

				delta_.first.insert(*it);
				return false;
			}
		}

		bool update(const typename Table::value_type& tup)
		{
			if (!remove(tup))
				return false;

			delta_.first.insert(tup);
			return true;
		}

	private:
		bool remove_from_plus(const sort_tuple& sub)
		{
			plus_iterator pit;

            if (!rel::find(delta_.first, sub, pit))
				return false;

			delta_.first.remove(pit);
			return true;
		}

	private:
		Table table_;
		typename table_delta<Table>::delta_type delta_;
	};

	class transaction
	{
		typedef std::set<table_proxy*, id_less> set_type;

	public:
		~transaction()
		{
			std::for_each(proxies_.begin(), proxies_.end(), deleter());
		}
		template<class Exprs>
			void commit(Exprs& exprs)
		{
			exprs.on_before_commit(*this);
			commit();
			exprs.on_after_commit();
		}
		void commit()
		{
			std::for_each(proxies_.begin(), proxies_.end(), 
				boost::mem_fn(&table_proxy::commit));

			clear();
		}
		void rollback()
		{
			clear();
		}
		template<class T> 
			bool insert(const T& t, const typename T::value_type& tuple)
		{
            BOOST_STATIC_ASSERT((is_same<typename T::tag, table_tag>::value));
			return add(t)->insert(tuple);
		}
		template<class T>
			bool remove(const T& t, const typename T::value_type& tuple)
		{
            BOOST_STATIC_ASSERT((is_same<typename T::tag, table_tag>::value));
			return add(t)->remove(tuple);
		}

		template<class It>
			bool remove(const It& it)
		{
            BOOST_STATIC_ASSERT((is_same<typename It::relation_type::tag, table_tag>::value));
			return add(*(it.relation()))->remove(it); //DA
		}

		template<class T>
			bool update(const T& t, const typename T::value_type& tuple)
		{
            BOOST_STATIC_ASSERT((is_same<typename T::tag, table_tag>::value));
			return add(t)->update(tuple);
		}

		template<class T, class Tuple>
            bool update(const T& t, const typename T::const_iterator& it, const Tuple& tuple)
		{
            BOOST_STATIC_ASSERT((is_same<typename T::tag, table_tag>::value));
			return add(t)->update(it,tuple); //DA
		}

		template<class T>
			typename table_delta<T>::delta_type delta(const T& t) const
		{
            BOOST_STATIC_ASSERT((is_same<typename T::tag, table_tag>::value));

            std::auto_ptr<table_proxy> ap(new table_proxy_impl<T>(t));
			set_type::const_iterator i = proxies_.find(ap.get());

			typedef typename table_delta<T>::plus_type plus_type;
			typedef typename table_delta<T>::minus_type minus_type;

			if (i == proxies_.end())
				return std::make_pair(plus_type(),minus_type(t));

			table_proxy_impl<T>* p = 
				static_cast<table_proxy_impl<T>*>(*i);

			assert(p != 0);
			return p->delta_;
		}
		template<class T>
			bool has_delta(const T& t) const
		{
            BOOST_STATIC_ASSERT((is_same<typename T::tag, table_tag>::value));
			std::auto_ptr<table_proxy> ap(new table_proxy_impl<T>(t));
			return proxies_.find(ap.get()) != proxies_.end();
		}
	private:
		template<class T>
			table_proxy_impl<T>* add(const T& t)
		{
            BOOST_STATIC_ASSERT((is_same<typename T::tag, table_tag>::value));

            table_proxy_impl<T> tpi(t);
			set_type::iterator it = proxies_.find(&tpi);

			if (it != proxies_.end())
				return static_cast<table_proxy_impl<T>*>(*it);

			std::pair<set_type::iterator, bool> insert_result = 
				proxies_.insert(new table_proxy_impl<T>(t));

			assert(insert_result.second);

			table_proxy_impl<T>* p = 
				static_cast<table_proxy_impl<T>*>(*insert_result.first);

			assert(p != 0);
			return p;
/*
			table_proxy* ip = new table_proxy_impl<table<Info> >(t);
			std::pair<set_type::iterator, bool> insert_result = proxies_.insert(ip);
			
			if (!insert_result.second)
				delete ip;

			table_proxy_impl<table<Info> >* p = 
				static_cast<table_proxy_impl<table<Info> >*>(*insert_result.first);

			assert(p != 0);
			return p;
*/
		}
		void clear()
		{
			std::for_each(proxies_.begin(), proxies_.end(), deleter());
			proxies_.clear();
		}

	private:
		set_type proxies_;
	};
}}

#endif//BOOST_RTL_TRANSACTION_HPP_INCLUDED
