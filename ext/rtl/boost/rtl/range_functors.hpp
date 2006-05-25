#ifndef BOOST_RTL_RANGE_FUNCTORS_HPP_INCLUDED
#define BOOST_RTL_RANGE_FUNCTORS_HPP_INCLUDED

namespace boost { namespace rel {

	struct begin
	{
		template<class T>
			typename T::const_iterator operator()(const T& t) const
		{
			return t.begin();
		}
		template<class It, class T>
			typename T::const_iterator operator()(const It&, const T& t) const
		{
			return t.begin();
		}
	};

	struct end
	{
		template<class T>
			typename T::const_iterator operator()(const T& t) const
		{
			return t.end();
		}
		template<class It, class T>
			typename T::const_iterator operator()(const It&, const T& t) const
		{
			return t.end();
		}
	};

	template<class FieldList>
		class lower_bound_t
	{
	public:
		template<class It, class Table>
			typename Table::const_iterator operator()(const It& it, const Table& t) const
		{
			typedef typename list_prefix<
				typename Table::sort_list, 
				rel::size<FieldList>::value
			>::type sub_list;

			row<sub_list> sub;
			copy_different_tuples<FieldList, sub_list, rel::size<FieldList>::value>(it, sub);
			return t.lower_bound(sub);
		}
	};

	template<class FieldList>
		class lower_bound_t<row<FieldList> >
	{
	public:
		lower_bound_t(const row<FieldList>& sub)
			: sub_(sub)
		{}
		template<class T>
			typename T::const_iterator operator()(const T& t) const
		{
			return t.lower_bound(sub_);
		}
	private:
		row<FieldList> sub_;
	};

	template<class FieldList>
		lower_bound_t<FieldList> lower_bound()
	{
		return lower_bound_t<FieldList>();
	}

	template<class FieldList>
		lower_bound_t<row<FieldList> > lower_bound(const row<FieldList>& sub)
	{
		return lower_bound_t<row<FieldList> >(sub);
	}

	template<class FieldList>
		class upper_bound_t
	{
	public:
		template<class It, class Table>
			typename Table::const_iterator operator()(const It& it, const Table& t) const
		{
			typedef typename list_prefix<
				typename Table::sort_list, 
				rel::size<FieldList>::value
			>::type sub_list;

			row<sub_list> sub;
			copy_different_tuples<FieldList, sub_list, rel::size<FieldList>::value>(it, sub);
			return t.upper_bound(sub);
		}
	};

	template<class FieldList>
		class upper_bound_t<row<FieldList> >
	{
	public:
		upper_bound_t(const row<FieldList>& sub)
			: sub_(sub)
		{}
		template<class T>
			typename T::const_iterator operator()(const T& t) const
		{
			return t.upper_bound(sub_);
		}
	private:
		row<FieldList> sub_;
	};

	template<class FieldList>
		upper_bound_t<FieldList> upper_bound()
	{
		return upper_bound_t<FieldList>();
	}

	template<class FieldList>
		upper_bound_t<row<FieldList> > upper_bound(const row<FieldList>& sub)
	{
		return upper_bound_t<row<FieldList> >(sub);
	}
}}

#endif//BOOST_RTL_RANGE_FUNCTORS_HPP_INCLUDED
