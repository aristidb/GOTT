// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_GROUPBY_DELTA_HPP_INCLUDED
#define BOOST_RTL_GROUPBY_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>
#include <boost/rtl/groupby.hpp>
#include <boost/rtl/range_join.hpp>
#include <boost/rtl/projection.hpp>
#include <boost/rtl/rename.hpp>
#include <boost/rtl/list.hpp>
#include <boost/rtl/table.hpp>
#include <boost/rtl/merge.hpp>
#include <boost/mpl/transform_view.hpp>

#include <boost/mpl/and.hpp>
#include <boost/mpl/logical.hpp>

#include <boost/rtl/groupby_functors.hpp>

namespace boost { namespace rel {

    template<class R, class groupby_tag> 
    struct traits_impl
	{
        typedef typename table_type<R>::type minus_type;
        typedef typename table_type<R>::type plus_type;
	};

    template<class T> 
    struct groupper_traits
    {
        typedef boost::mpl::false_ supports_fast_plus;
    };

    struct all_support_fast_plus_functor
    {
        template<class State, class T>
        struct apply : boost::mpl::and_<
            State, 
            typename groupper_traits<T>::supports_fast_plus
        >{};
    };

    template<class GroupList>
    struct all_support_fast_plus : boost::mpl::fold<
        GroupList, 
        boost::mpl::true_, 
        all_support_fast_plus_functor
    >{};

	struct gby_plus;
    struct gby_minus;

    template<class Tup, class It>
    class get_groupper
    {
    public:
        get_groupper(const It& it, Tup& tup, bool& is_valid)
            : it_(it)
            , tup_(tup)
            , is_valid_(is_valid)
        {}
        template<class C>
        void operator()(const C&) const
        {
            is_valid_ &= groupper_traits<C>::apply_delta(
                tup_[C()],
                it_[C()], 
                it_[alias<C, gby_minus>()], 
                it_[alias<C, gby_plus>()]
                );
        }
    private:
        const It& it_;
        Tup& tup_;
        bool& is_valid_;
    };

    template<class T, class R, class KeyList, class GroupList>
        void populate_plus(T& t, const R& r, KeyList, GroupList)
    {
        for (typename R::const_iterator it =
            r.begin(); it != r.end(); ++it)
        {
            typename T::value_type tup;
            bool is_valid = true;
            
            boost::mpl::for_each<GroupList>(
                get_groupper<typename T::value_type, typename R::const_iterator>(it, tup, is_valid)
                );
            
            if (is_valid)
            {
                copy_fields<KeyList>(it, tup);
                t.insert(tup);
            }
        }
    }

    template<class A> 
    struct rename_1_field
    {
        template<class T> 
        struct apply
        {
            typedef alias<T, A> type;
        };
    };

    template<class T, class A>
    struct rename_list : boost::mpl::transform_view<
        T,
        rename_1_field<A>
    >{};

    template<class R>
    struct groupby_delta_helper
    {
	    typedef table<table_info<typename R::key_list> > affected_groups_t;

        typename traits<typename R::arg_type>::delta_type arg_delta_;
        affected_groups_t affected_groups_;
        typename traits<R>::minus_type minus_;

        groupby_delta_helper(const R& gby, const transaction& tr)
            : arg_delta_(delta(gby.argument(), tr))
        {
            affected_groups_.append(
                projection<typename R::key_list>(Union(arg_delta_.first, arg_delta_.second))
            );

            minus_.append(
			    manual_rename<typename R::field_list>(
                    groupby<R::prefix_size, typename R::group_list>(
                        equal_join<typename rename_list<typename R::key_list, void>::type>(
                            auto_rename<void>(affected_groups_),
                            gby.argument()
                        )
 				    )
			    )
            );
        }
    };
    
    template<class R>
    typename traits<R>::delta_type groupby_delta(const R& gby, const transaction& tr, boost::mpl::false_)
	{
        typedef groupby_delta_helper<R> helper_type;

        helper_type helper(gby, tr);

        typename traits<R>::plus_type plus;

        plus.append(
            manual_rename<typename R::field_list>(
                groupby<R::prefix_size, typename R::group_list>(
				    equal_join<typename rename_list<typename R::key_list, void>::type>(
                        auto_rename<void>(helper.affected_groups_),
                        modified(gby.argument(), tr)
                    )
				)
			)
        );

		return std::make_pair(plus, helper.minus_);
	}

    template<class R>
    typename traits<R>::delta_type groupby_delta(const R& gby, const transaction& tr, boost::mpl::true_)
	{
        // helper (minus)
        typedef groupby_delta_helper<R> helper_type;

        helper_type helper(gby, tr);

        // this table has grouppers set to nulls
        typename traits<R>::plus_type fill;

        for (typename helper_type::affected_groups_t::const_iterator it =
            helper.affected_groups_.begin(); it != helper.affected_groups_.end(); ++it)
        {
            typename traits<R>::plus_type::value_type tup;
            copy_fields<typename helper_type::affected_groups_t::field_list>(it, tup);
            fill.insert(tup);
        }

        // plus
        typename traits<R>::plus_type plus;

        populate_plus(
            plus,
            merge<R::prefix_size>(
                Union(helper.minus_, fill),
                merge<R::prefix_size>(
                    auto_rename<gby_plus>(Union(groupby<R::prefix_size, typename R::group_list>(helper.arg_delta_.first), fill)),
                    auto_rename<gby_minus>(Union(groupby<R::prefix_size, typename R::group_list>(helper.arg_delta_.second), fill))
                    )),
            typename R::key_list(), 
            typename R::group_list()
            );

		return std::make_pair(plus, helper.minus_);
	}

    template<class R>
	typename traits<R>::delta_type delta(const R& gby, const transaction& tr, groupby_tag)
    {
        typename all_support_fast_plus<typename R::group_list>::type selector; 
        return groupby_delta(gby, tr, selector);
    }

    // groupper traits specializations

    template<class T> 
    struct groupper_traits<column_name<counter_t<T> > >
    {
        typedef boost::mpl::true_ supports_fast_plus;
        //typedef boost::mpl::false_ supports_fast_plus;

        static bool apply_delta(T& result, T original, T minus, T plus)
        {
            result = original - minus + plus;
            return result != 0;
        }
    };
}}

#endif//BOOST_RTL_GROUPBY_DELTA_HPP_INCLUDED
