#include "math/fixed_vector.hpp"

namespace gott{namespace math{ 

template<typename ScalarT> struct pick_next_float_type;
#define PICK_NEXT_FLOAT_TYPE(TYPEIN,TYPEOUT) template<> struct pick_next_float_type<TYPEIN>{ typedef TYPEOUT type; };

PICK_NEXT_FLOAT_TYPE(int,float)
PICK_NEXT_FLOAT_TYPE(long,float)
PICK_NEXT_FLOAT_TYPE(char,float)
PICK_NEXT_FLOAT_TYPE(short,float)
PICK_NEXT_FLOAT_TYPE(unsigned int,float)
PICK_NEXT_FLOAT_TYPE(unsigned long,float)
PICK_NEXT_FLOAT_TYPE(unsigned char,float)
PICK_NEXT_FLOAT_TYPE(unsigned short,float)
PICK_NEXT_FLOAT_TYPE(float,float)
PICK_NEXT_FLOAT_TYPE(double,double)
PICK_NEXT_FLOAT_TYPE(long double,long double)

#undef PICK_NEXT_FLOAT_TYPE

template<typename T,size_t N> struct pick_vector;
template<typename T> struct pick_vector<1> { typedef vector1<T> type; };
template<typename T> struct pick_vector<2> { typedef vector2<T> type; };
template<typename T> struct pick_vector<3> { typedef vector3<T> type; };
template<typename T> struct pick_vector<4> { typedef vector4<T> type; };

}}

