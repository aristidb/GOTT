// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_COVER_HPP_INCLUDED
#define BOOST_RTL_COVER_HPP_INCLUDED

#include <boost/rtl/copy_ptr.hpp>
#include <boost/rtl/iterator_root.hpp>
#include <boost/rtl/traverse.hpp>
#include <boost/rtl/list.hpp>
#include <boost/rtl/expression_registry.hpp>

namespace boost { namespace rel {

	class transaction;

	template<class Tuple, class SortList, class Keys> class cover_t;

	template<class Tuple, class SortList>
	class cover_abstract_iterator_helper
	{
	public:
		typedef Tuple value_type;
		typedef cover_abstract_iterator_helper AbstractIterator;
		virtual AbstractIterator* clone() const = 0;
		virtual void increment() = 0;
		virtual void decrement() = 0;
		virtual void get(int n, void* p) const = 0;
		virtual bool operator ==(const AbstractIterator& other) const = 0;
		virtual ~cover_abstract_iterator_helper() {}
	};

	template<class Table>
	class cover_concrete_iterator_helper
		: public cover_abstract_iterator_helper
			<
				typename Table::value_type,
				typename Table::sort_list
			>
	{
	public:
		typedef typename cover_concrete_iterator_helper::AbstractIterator AbstractIterator; 
		typedef typename cover_concrete_iterator_helper::value_type value_type; 

		cover_concrete_iterator_helper() : itr_() {}
		cover_concrete_iterator_helper(const typename Table::const_iterator& itr) : itr_(itr) {}
		cover_concrete_iterator_helper(const cover_concrete_iterator_helper& other) : itr_(other.itr_) {}

		virtual AbstractIterator* clone() const
		{
			return new cover_concrete_iterator_helper(*this);
		}

		virtual bool operator ==(const AbstractIterator& other) const
		{
			return itr_ == static_cast<const cover_concrete_iterator_helper*>(&other)->itr_;
		}

		virtual void increment()
		{
			++itr_;
		}

		virtual void decrement()
		{
			--itr_;
		}

	private:
		class GetFunctor
		{
		public:
			GetFunctor(const typename Table::const_iterator& it, int n, void* p) : it_(it), n_(n), p_(p) {}
			template<class Column> bool operator()(Column&)
			{
				if (n_ != list_index_of<typename value_type::field_list, Column>::value) return true;
				else
				{
					*static_cast<typename Column::type*>(p_) = it_.get(Column());
					return false;
				}
			}
		private:
			const typename Table::const_iterator& it_;
			int n_;
			void* p_;
		};

	public:
		virtual void get(int n, void* p) const
		{
			GetFunctor functor(itr_,n, p);
			breakable_for_each<typename value_type::field_list>(functor);
		}

		virtual ~cover_concrete_iterator_helper()
		{
		}

		typename Table::const_iterator itr_;
	};

	template<class Tuple, class SortList, class Keys>
	class cover_iterator
		: public iterator_root
		<
			cover_iterator<Tuple, SortList, Keys>,
			cover_t<Tuple, SortList, Keys>
		>
	{
		typedef iterator_root<cover_iterator,cover_t<Tuple, SortList, Keys> > IteratorRoot;
		friend class iterator_root<cover_iterator,cover_t<Tuple, SortList, Keys> >;
		friend class cover_abstract_iterator_helper<Tuple, SortList>;
		friend class cover_t<Tuple,SortList,Keys>;
		typedef cover_abstract_iterator_helper<Tuple, SortList> AbstractIterator;
	public:
		typedef typename cover_iterator::relation_type relation_type;
		typedef typename cover_iterator::value_type value_type;

		cover_iterator() {}

		template<class Column>
			typename Column::type
		get_internal(const Column&) const 
		{
			typename Column::type temp;
			enum {ni = list_index_of<typename value_type::field_list,Column>::value};
			(*itr_).get(ni,&temp);
			return temp;
		}

		bool operator == (const cover_iterator& other) const 
		{
			if (itr_.get() == 0)
				return other.itr_.get() == 0;
			else if (other.itr_.get() == 0)
				return false;
			else
				return *itr_ == *other.itr_;
		}

	private:
		cover_iterator(AbstractIterator* itr, const relation_type* o) 
			: IteratorRoot(o)
			, itr_(itr) 
		{}

		void increment()
		{
			(*itr_).increment();
		}
		void decrement()
		{
			(*itr_).decrement();
		}
	private:
		copy_ptr<AbstractIterator,true> itr_;
	};

	struct functor_proxy
	{
		virtual void uncover() const = 0; 
	};

	template<class Tuple, class SortList>
	class cover_abstract_helper
	{
		friend class cover_abstract_iterator_helper<Tuple,SortList>;
	public:
		typedef cover_abstract_iterator_helper<Tuple,SortList> AbstractIterator;
		typedef cover_abstract_helper<Tuple,SortList> AbstractRelation;
		virtual AbstractRelation* clone() const = 0;
		virtual AbstractIterator* begin() const = 0;
		virtual AbstractIterator* end() const = 0;

		virtual std::pair<AbstractIterator*,AbstractIterator*> equal_range(const tuple_base& sub, int n) const = 0;
		virtual AbstractIterator* lower_bound(const tuple_base& sub, int n) const = 0;
		virtual AbstractIterator* upper_bound(const tuple_base& sub, int n) const = 0;
		virtual ~cover_abstract_helper() {}
		virtual AbstractRelation* getMinus(const transaction& tr) const  = 0;
		virtual AbstractRelation* getPlus(const transaction& tr) const = 0;

		virtual void register_expression(expression_registry* reg) const = 0;
	};

	template<class Table>
	class cover_concrete_helper
		: public cover_abstract_helper
		<
			typename Table::value_type,
			typename Table::sort_list
		>
	{
		typedef cover_abstract_helper<
			typename Table::value_type,
			typename Table::sort_list
		> base_type;

	public:
		typedef typename base_type::AbstractRelation AbstractRelation;
		typedef typename base_type::AbstractIterator AbstractIterator;

		cover_concrete_helper(const Table& tab) : tab_(tab)
		{
		}

		cover_concrete_helper(const cover_concrete_helper& other) : tab_(other.tab_)
		{
		}

		virtual ~cover_concrete_helper() {}

		virtual AbstractRelation* clone() const
		{
			return new cover_concrete_helper(*this);
		}

		virtual AbstractIterator* begin() const
		{
			return new cover_concrete_iterator_helper<Table>(tab_.begin());
		}

		virtual AbstractIterator* end() const
		{
			return new cover_concrete_iterator_helper<Table>(tab_.end());
		}

		class EqualRangeFunctor
		{
		public:
			EqualRangeFunctor(
				const Table& tab,
				const tuple_base& sub,
				int fields_in_tuple,
				typename Table::const_iterator* b,
				typename Table::const_iterator* e
			)
				: tab_(&tab)
				, sub_(&sub)
				, fields_in_tuple_(fields_in_tuple)
				, b_(b)
				, e_(e)
			{
			}
			template<class List>
				bool operator()(const List&)
			{
                enum {fields_in_List = rel::size<List>::value};
				if (fields_in_List != fields_in_tuple_) return true;
				else
				{
					range_t<typename Table::const_iterator> p = 
						tab_->equal_range(*(static_cast<const row<List>*>(sub_)));
					*b_ = p.begin().base();
					*e_ = p.end().base();
					return false;
				}
			}
		private:
			const Table* tab_;
			const tuple_base* sub_;
			int fields_in_tuple_;
			typename Table::const_iterator* b_;
			typename Table::const_iterator* e_;
		};

		virtual std::pair<AbstractIterator*,AbstractIterator*> equal_range(const tuple_base& sub, int n) const
		{
			cover_concrete_iterator_helper<Table>* low = new cover_concrete_iterator_helper<Table>();
			cover_concrete_iterator_helper<Table>* high = new cover_concrete_iterator_helper<Table>();
			EqualRangeFunctor functor(tab_, sub, n, &low->itr_, &high->itr_);
			breakable_for_each<typename generate_prefixes<typename Table::sort_list>::type>(functor);
			return std::make_pair(low,high);
		}

		class LowerBoundFunctor
		{
		public:
			LowerBoundFunctor(
				const Table& tab,
				const tuple_base& sub,
				int fields_in_tuple,
				typename Table::const_iterator* b
			)
				: tab_(&tab)
				, sub_(&sub)
				, fields_in_tuple_(fields_in_tuple)
				, b_(b)
			{
			}
			template<class List>
				bool operator()(const List&)
			{
				enum {fields_in_List = rel::size<List>::value};
				if (fields_in_List != fields_in_tuple_) return true;
				else
				{
					*b_ = tab_->lower_bound(*static_cast<const row<List>*>(sub_));
					return false;
				}
			}
		private:
			const Table* tab_;
			const tuple_base* sub_;
			int fields_in_tuple_;
			typename Table::const_iterator* b_;
		};

		virtual AbstractIterator* lower_bound(const tuple_base& sub, int n) const
		{
			cover_concrete_iterator_helper<Table>* low = new cover_concrete_iterator_helper<Table>();
			LowerBoundFunctor functor(tab_,sub,n,&low->itr_);
			breakable_for_each<typename generate_prefixes<typename Table::sort_list>::type>(functor);
			return low;
		}

		class UpperBoundFunctor
		{
		public:
			UpperBoundFunctor(
				const Table& tab,
				const tuple_base& sub,
				int fields_in_tuple,
				typename Table::const_iterator* e
			)
				: tab_(&tab)
				, sub_(&sub)
				, fields_in_tuple_(fields_in_tuple)
				, e_(e)
			{
			}

			template<class List>
				bool operator()(const List&)
			{
				enum {fields_in_List = rel::size<List>::value};
				if (fields_in_List != fields_in_tuple_) return true;
				else
				{
					*e_ = tab_->upper_bound(*static_cast<const row<List>*>(sub_));
					return false;
				}
			}
		private:
			const Table* tab_;
			const tuple_base* sub_;
			int fields_in_tuple_;
			typename Table::const_iterator* e_;
		};

		virtual AbstractIterator* upper_bound(const tuple_base& sub, int n) const
		{
			cover_concrete_iterator_helper<Table>* high = new cover_concrete_iterator_helper<Table>();
			UpperBoundFunctor functor(tab_,sub,n,&high->itr_);
			breakable_for_each<typename generate_prefixes<typename Table::sort_list>::type>(functor);
			return high;
		}

		virtual AbstractRelation* getMinus(const transaction& tr) const {return 0;}
		virtual AbstractRelation* getPlus(const transaction& tr) const {return 0;}

		virtual void register_expression(expression_registry* reg) const
		{
			reg->add(tab_);
		}

	protected:
		Table tab_;
	};

    struct cover_tag {};

    template<class Tuple, class SortList, class Keys> class cover_t
	{
	public:
        typedef cover_tag tag;

		typedef typename Tuple::field_list field_list;
		typedef SortList sort_list;
		typedef row<field_list> value_type;
		typedef cover_iterator<Tuple,SortList,Keys> const_iterator;
		typedef cover_abstract_helper<Tuple,SortList> AbstractRelation;
		typedef cover_abstract_iterator_helper<Tuple,SortList> AbstractIterator;
		typedef Keys keys;

		cover_t(const cover_t& other) : hlp_(other.hlp_) {}
		cover_t(AbstractRelation* hlp) : hlp_(hlp) {}

		~cover_t() {}

		const_iterator begin() const
		{
			return const_iterator(hlp_->begin(),this);
		}
		const_iterator end() const
		{
			return const_iterator(hlp_->end(),this);
		}

		template<class SubTuple>
			range_t<const_iterator> equal_range(const SubTuple& sub) const
		{
			const int depth = rel::size<typename SubTuple::field_list>::value;
			std::pair<AbstractIterator*,AbstractIterator*> p = hlp_->equal_range(sub,depth);
			const_iterator b(p.first,this);
			const_iterator e(p.second,this);
			return range(b,e);
		}

		template<class SubTuple>
			const_iterator lower_bound(const SubTuple& sub) const
		{
			const int depth = rel::size<typename SubTuple::field_list>::value;
			AbstractIterator* i = hlp_->lower_bound(sub,depth);
			const_iterator b = const_iterator(i,this);
			return b;
		}

		template<class SubTuple>
			const_iterator upper_bound(const SubTuple& sub) const
		{
			const int depth = rel::size<typename SubTuple::field_list>::value;
			AbstractIterator* i = hlp_->upper_bound(sub,depth);
			const_iterator e = const_iterator(i,this);
			return e;
		}

		cover_t getPlus(const transaction& tr) const
		{
			return cover_t(hlp_->getPlus(tr));
		}

		cover_t getMinus(const transaction& tr) const
		{
			return cover_t(hlp_->getMinus(tr));
		}

		void register_expression(expression_registry* reg) const
		{
			hlp_->register_expression(reg);
		}
		
		template<class F>
			void traverse_args(F f) const
		{}

	private:
		copy_ptr<AbstractRelation,true> hlp_;
	};

	#if defined(BOOST_MPL_MSVC_ETI_BUG)
	template<> class cover_t<int,int,int>
	{
	public:
		typedef int field_list;
	};
	#endif

}}

#endif//BOOST_RTL_COVER_HPP_INCLUDED
