// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_COLUMN_HPP_INCLUDED
#define BOOST_RTL_COLUMN_HPP_INCLUDED

#define BOOST_RTL_DEFINE_COLUMN(T, Name)\
	struct Name : public boost::rel::column_name<boost::rel::column<Name, T > >{};

// the following macro is depricated and will be removed soon
#define BOOST_RTL_COLUMN(Name, T) BOOST_RTL_DEFINE_COLUMN(T, Name)

namespace boost { namespace rel {

	template<class Impl> 
		struct column_name
	{
		typedef Impl impl_type;
		typedef typename Impl::type type;
		typedef typename Impl::reference reference;
		typedef typename Impl::const_reference const_reference;
	};

	// column

	template<class T>
		struct default_value
	{
		static T get()
		{
			return T();
		}
	};
	template <class Name, class T> 
		class column
	{
	public:
		typedef T type;
		typedef T& reference;
		typedef const T& const_reference;
		enum {ctg = 0};
	public:
		column() 
			: m_value(default_value<T>::get())
		{}
		column(const T& value) 
			: m_value(value)
		{}

		const_reference value() const {return m_value;}  
		reference value() {return m_value;}

	private:
		T m_value;
	};

	// alias

	template <class Col, class Alias = null_type> 
		struct alias : public column_name<column<alias<Col, Alias>, typename Col::type> >
	{
		typedef Col aliased_type;
		enum {ctg = 2};
	};

	template<class Table>
		struct idxr : public column_name<column<idxr<Table>, typename Table::const_iterator> >
	{
		typedef Table table_type;
		typedef typename Table::value_type value_type;

		enum {ctg = 3};
	};

}}

#endif//BOOST_RTL_COLUMN_HPP_INCLUDED
