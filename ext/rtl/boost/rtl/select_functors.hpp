// Copyright (C) 2002 Arkadiy Vertleyb, Dmitriy Arapov
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.

#ifndef BOOST_RTL_SELECT_FUNCTORS_HPP_INCLUDED
#define BOOST_RTL_SELECT_FUNCTORS_HPP_INCLUDED

#define SELECT_FUNCTOR(name, op)\
	template <class C> class name	{\
	public:\
		name (typename C::const_reference x) : x_(x) {}\
		template <class Tuple> bool operator()(const Tuple& tp) const{\
			return BOOST_RTL_FIELD(tp, C) op x_;\
		}\
	private:\
		typename C::type x_;\
	};\
	template <class C1, class C2> struct name##2 {\
		template <class Tuple> bool operator()(const Tuple& tp) const{\
			return BOOST_RTL_FIELD(tp, C1) op BOOST_RTL_FIELD(tp, C2);\
		}\
	}

#define SELECT_CONNECTOR(name, op)\
	template <class P1, class P2> class name##_t {\
	public:\
		name##_t (P1 p1, P2 p2) : p1_(p1), p2_(p2) {}\
		template <class Tuple> bool operator()(const Tuple& tp) const{\
			return p1_(tp) op p2_(tp);\
		}\
	private:\
		P1 p1_;\
		P2 p2_;\
	};\
	template <class P1, class P2> name##_t<P1, P2> name(P1 p1, P2 p2){\
		return name##_t<P1, P2>(p1, p2);\
	}

namespace boost { namespace rel {

	SELECT_FUNCTOR(gt, >);
	SELECT_FUNCTOR(ge, >=);
	SELECT_FUNCTOR(eq, ==);
	SELECT_FUNCTOR(ne, !=);
	SELECT_FUNCTOR(le, <=);
	SELECT_FUNCTOR(lt, <);

	SELECT_CONNECTOR(Or, ||);
	SELECT_CONNECTOR(And, &&);
}}

#endif//BOOST_RTL_SELECT_FUNCTORS_HPP_INCLUDED
