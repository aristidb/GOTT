// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_EXPRESSION_REGISTRY_HPP_INCLUDED
#define BOOST_RTL_EXPRESSION_REGISTRY_HPP_INCLUDED

#include <boost/rtl/traverse.hpp>
#include <boost/rtl/transaction.hpp>

namespace boost { namespace rel {

	// node_proxy

	struct node_proxy
	{
		virtual ~node_proxy(){}
		virtual ptrdiff_t id() const = 0;
		virtual void on_before_commit(transaction& tr) = 0;
		virtual void on_after_commit() = 0;
		typedef boost::shared_ptr<node_proxy> ptr;
	};

	// key_index_proxy_impl

	template<class Idx>
		class key_index_proxy_impl : public node_proxy
	{
	public:
		key_index_proxy_impl(const Idx& idx)
			: idx_(idx)
		{}
		virtual ptrdiff_t id() const
		{
			return idx_.id();
		}
		virtual void on_before_commit(transaction& tr)
		{
			update_index(tr);
		}
		virtual void on_after_commit()
		{}
	private:
		virtual void update_index(transaction& tr)
		{
            typedef typename traits<Idx>::delta_type delta_type;
			typedef typename traits<Idx>::plus_type::idx plus_type;
			typedef typename traits<Idx>::minus_type::idx minus_type;

			delta_type d = delta(idx_, tr);
            plus_type plus = d.first.internal_table();
			minus_type minus = d.second.internal_table();

			typename minus_type::const_iterator minus_end = minus.end();
            for (typename minus_type::const_iterator it1 = minus.begin(); it1 != minus_end; ++it1)
			{
				row<typename Idx::idx::sort_list> sub;
				copy_fields<typename Idx::idx::sort_list>(it1, sub);
				idx_.internal_table().remove(idx_.internal_table().lower_bound(sub));
			}

			typename plus_type::const_iterator plus_end = plus.end();
			for (typename plus_type::const_iterator it2 = plus.begin(); it2 != plus_end; ++it2)
				idx_.internal_table().insert(*it2);
		}
		Idx idx_;
	};

	// node_proxy_impl

	template<class Idx>
		class node_proxy_impl : public node_proxy
	{
	public:
		node_proxy_impl(const Idx& idx)
			: idx_(idx)
		{}
		virtual ptrdiff_t id() const
		{
			return idx_.id();
		}
		virtual void on_before_commit(transaction& tr)
		{}
		virtual void on_after_commit()
		{
			idx_.refill();
		}
	private:
		Idx idx_;
	};

	// ctreate_node_proxy

    struct key_index_tag;
    struct iterator_index_tag;
    struct cache_tag;
    struct cover_tag;

	// expression_registry_traverse_functor
	
	class expression_registry;

    class expression_registry_traverse_functor
	{
	public:
		expression_registry_traverse_functor(expression_registry* reg)
			: reg_(reg)
		{}

		template<class R>
			void operator()(R& r, const key_index_tag& tag) const;

        template<class R>
			void operator()(R& r, const iterator_index_tag& tag) const;

        template<class R>
			void operator()(R& r, const cache_tag& tag) const;

        template<class R>
			void operator()(R& r, const cover_tag&) const
		{
			r.register_expression(reg_);
		}

        template<class R>
    		void operator()(R& r, ...) const
		{}

	private:
		expression_registry* reg_;
	};

	// expression_registry

	class expression_registry
	{
	public:
		template<class E>
			void add(const E& exp)
		{
			traverse(exp, expression_registry_traverse_functor(this));
		}
		void add_node(node_proxy::ptr proxy)
		{
			for (std::vector<node_proxy::ptr>::iterator it =
				proxies_.begin(); it != proxies_.end(); ++it)
			{
				if ((*it)->id() == proxy->id())
					proxies_.erase(it);
			}
			proxies_.push_back(proxy);
		}
		void on_before_commit(transaction& tr)
		{
			for (std::vector<node_proxy::ptr>::iterator it =
				proxies_.begin(); it != proxies_.end(); ++it)
			{
				(*it)->on_before_commit(tr);
			}
		}
		void on_after_commit()
		{
			for (std::vector<node_proxy::ptr>::reverse_iterator it =
				proxies_.rbegin(); it != proxies_.rend(); ++it)
			{
				(*it)->on_after_commit();
			}
		}
		void print_ids()
		{
			for (std::vector<node_proxy::ptr>::iterator it =
				proxies_.begin(); it != proxies_.end(); ++it)
			{
				std::cout << (double)(*it)->id() << std::endl;
			}
		}
	private:
		std::vector<node_proxy::ptr> proxies_;
	};

	template<class R>
    void expression_registry_traverse_functor::operator()(R& r, const key_index_tag& tag) const
    {
        reg_->add_node(node_proxy::ptr(new key_index_proxy_impl<R>(r)));
    }

    template<class R>
    void expression_registry_traverse_functor::operator()(R& r, const iterator_index_tag& tag) const
    {
    	reg_->add_node(node_proxy::ptr(new node_proxy_impl<R>(r)));
    }

    template<class R>
    void expression_registry_traverse_functor::operator()(R& r, const cache_tag& tag) const
    {
    	reg_->add_node(node_proxy::ptr(new node_proxy_impl<R>(r)));
    }

}}

#endif//BOOST_RTL_EXPRESSION_REGISTRY_HPP_INCLUDED
