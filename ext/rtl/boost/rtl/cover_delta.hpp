// Copyright (C) 2002 Dmitriy Arapov, Arkadiy Vertleyb
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_COVER_DELTA_HPP_INCLUDED
#define BOOST_RTL_COVER_DELTA_HPP_INCLUDED

#include <boost/rtl/traits.hpp>

#define CONCRETE_HELPER cover_concrete_helper_with_deltas
//#define CONCRETE_HELPER cover_concrete_helper

#include <boost/rtl/cover.hpp>

namespace boost { namespace rel {

	template<class Table>
		class cover_concrete_helper_with_deltas : public cover_concrete_helper<Table>
	{
	public:
		typedef typename cover_concrete_helper<Table>::AbstractRelation AbstractRelation;
        using cover_concrete_helper<Table>::tab_;

		cover_concrete_helper_with_deltas(const Table& tab) : cover_concrete_helper<Table>(tab)
		{
		}
		cover_concrete_helper_with_deltas(const cover_concrete_helper_with_deltas& other) : cover_concrete_helper<Table>(other)
		{
		}

		virtual ~cover_concrete_helper_with_deltas() 
		{
		}

		virtual AbstractRelation* clone() const
		{
			return new cover_concrete_helper_with_deltas(*this);
		}

	private:
		virtual AbstractRelation* getMinus(const transaction& tr) const
		{
			return new cover_concrete_helper<typename traits<Table>::minus_type>(delta(tab_,tr).second); 
		}

		virtual AbstractRelation* getPlus(const transaction& tr) const
		{
			return new cover_concrete_helper<typename traits<Table>::plus_type>(delta(tab_,tr).first);  
		}
	};
}

namespace rel
{
	template<class Table>
	struct cover_type
	{
		typedef cover_t
		<
			typename Table::value_type,
			typename Table::sort_list,
			typename Table::keys
		> type;
	};

	template<class Table> 
		typename cover_type<Table>::type 
	cover(const Table& tab)
	{
		return cover_t
		<
			typename Table::value_type, 
			typename Table::sort_list,
			typename Table::keys
		>
		(new CONCRETE_HELPER<Table>(tab));
	}

	template<class R> 
    struct traits_impl<R, cover_tag>
	{
		typedef cover_t<typename R::value_type, typename R::sort_list, typename R::keys> minus_type;
		typedef cover_t<typename R::value_type, typename R::sort_list, typename R::keys> plus_type;
	};

	template<class R>
	typename traits<R>::delta_type delta(const R& cv, const transaction& tr, cover_tag)
	{
        return std::make_pair(cv.getPlus(tr), cv.getMinus(tr));
	}

}} // namespace rtl

#undef CONCRETE_HELPER
#endif//BOOST_RTL_COVER_DELTA_HPP_INCLUDED

