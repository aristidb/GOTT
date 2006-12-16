
#ifndef GOTT_MATH_VERTEX_HPP_INCLUDED
#define GOTT_MATH_VERTEX_HPP_INCLUDED
#include <cctype>
#include "color.hpp"
namespace gott{namespace math{

template <typename Type>
struct vector1 {
	typedef Type channel_type;
	enum {size = 1 };
	Type data;

	vector1() {}
	vector1( Type d ) : data(d) {}
	void set( Type a ) { data=a;}

	static const vector1<Type> null_vector;
	static const vector1<Type> max_vector;
	
	
	Type & operator[]( size_t ) { return data; }
	Type const& operator[]( size_t ) const { return data; }

	template <typename TypeR>
	vector1<Type> & operator-=( vector1<TypeR> const& r ) { data -= r.data; return *this;}
	template <typename TypeR>
	vector1<Type> & operator+=( vector1<TypeR> const& r ) { data += r.data; return *this;}
	template <typename TypeR>
	vector1<Type> & operator*=( TypeR val ) { data *= val; return *this;}
	template <typename TypeR>
	vector1<Type> & operator/=( TypeR val ) { data /= val; return *this;}
	
	vector1<Type> & operator+=( Type const& r ) { data += r; return *this;}
	vector1<Type> & operator-=( Type const& r ) { data -= r; return *this;}
	vector1<Type> & operator*=( Type const& r ) { data *= r; return *this;}
	vector1<Type> & operator/=( Type const& r ) { data /= r; return *this;}

	template<typename T>
	operator T() const { return T(data);}

	template <typename T>
	bool operator != ( vector1<T> const &r ) const { return !(this == &r || r.data == data); }

	template <typename T>
	bool operator == ( vector1<T> const &r ) const { return (this == &r || r.data == data); }

	template <typename Functor>
	vector1<Type> applied( Functor fun ) const 
	{ return vector1<Type>( fun( data ) ); }

};


template <typename Type>
const vector1<Type> vector1<Type>::null_vector(Type(0));
template <typename Type>
const vector1<Type> vector1<Type>::max_vector( colour_channel_traits<Type>::max() );

template <typename TypeL,typename TypeR>
vector1<TypeL> operator+( vector1<TypeL> const& l, vector1<TypeR> const& r )
{
	return vector1<TypeL>( l.data + r.data );
}

template <typename TypeL,typename TypeR>
vector1<TypeL> operator-( vector1<TypeL> const& l, vector1<TypeR> const& r )
{
	return vector1<TypeL>( l.data - r.data );
}

template <typename Type>
vector1<Type> operator+( vector1<Type> const& l, Type const& r )
{
	return vector1<Type>( l.data + r );
}

template <typename Type>
vector1<Type> operator-( vector1<Type> const& l )
{
	return vector1<Type>( - l.data );
}


template <typename Type>
vector1<Type> operator-( vector1<Type> const& l, Type const& r )
{
	return vector1<Type>( l.data - r );
}

template <typename Type>
vector1<Type> operator*( vector1<Type> const& l, Type const& val )
{
	return vector1<Type>( l.data * val );
}

template <typename Type, typename TypeB>
vector1<TypeB> operator*( Type const& l, vector1<TypeB> const& r )
{
	return vector1<TypeB>( r.data * l );
}

template <typename Type>
inline Type operator*( vector1<Type> const& l, vector1<Type> const& r )
{
	return l[0]*r[0];
}

template <typename TypeL,typename TypeR>
vector1<TypeL> operator/( vector1<TypeL> const& l, TypeR val )
{
	return vector1<TypeL>( l.data / val );
}


template <typename Type>
struct vector2 
{
	typedef Type channel_type;
	enum {size = 2 };
	Type data[size];

	vector2() {}
	vector2( Type a, Type b ) { data[0]=a; data[1]=b; }
	void set( Type a, Type b ) { data[0]=a; data[1]=b;}

	static const vector2<Type> null_vector;
	static const vector2<Type> max_vector;
	
	Type & operator[]( size_t index ) { return data[index]; }
	Type const& operator[]( size_t index ) const { return data[index]; }

	template <typename TypeR>
	vector2<Type> & operator-=( vector2<TypeR> const& r ) {
		data[0] -= r.data[0];
		data[1] -= r.data[1];
		return *this;
	}
	template <typename TypeR>
	vector2<Type> & operator+=( vector2<TypeR> const& r )  {
		data[0] += r.data[0];
		data[1] += r.data[1];
		return *this;
	}
	template <typename TypeR>
	vector2<Type> & operator*=( TypeR val ) {
		data[0] *= val;
		data[1] *= val;
		return *this;
	}
	template <typename TypeR>
	vector2<Type> & operator/=( TypeR val ) {
		data[0] /= val;
		data[1] /= val;
		return *this;
	}

	template <typename T>
	bool operator != ( vector2<T> const &r ) const 
	{ return !( this == &r || ( r.data[0] == data[0] && r.data[1] == data[1] )); }

	template <typename T>
	bool operator == ( vector2<T> const &r ) const 
	{ return ( this == &r || ( r.data[0] == data[0] && r.data[1] == data[1] )); }

	template <typename Functor>
	vector2<Type> applied( Functor fun ) const 
	{ return vector2<Type>( fun( data[0] ), fun( data[1] ) ); }
};


	
template <typename Type>
const vector2<Type> vector2<Type>::null_vector(0,0);

template <typename Type>
const vector2<Type> vector2<Type>::max_vector(
		colour_channel_traits<Type>::max(),
		colour_channel_traits<Type>::max()
		);

template <typename TypeL,typename TypeR>
vector2<TypeL> operator+( vector2<TypeL> const& l, vector2<TypeR> const& r )
{
	return vector2<TypeL>( l[0] + r[0], l[1] + r[1] );
}

template <typename Type>
Type operator|( vector2<Type> const& l, vector2<Type> const& r )
{
	return l[0]*r[0]+l[1]*r[1];
}

template <typename TypeL,typename TypeR>
vector2<TypeL> operator-( vector2<TypeL> const& l, vector2<TypeR> const& r )
{
	return vector2<TypeL>(  l[0] - r[0], l[1] - r[1] );
}

template <typename Type>
vector2<Type> operator*( vector2<Type> const& l, Type val )
{
	return vector2<Type>( l[0] * val, l[1] * val );
}

template <typename Type>
inline vector2<Type> operator*(Type const& val, vector2<Type> const& r )
{
	return r*val;
}

template <typename Type>
inline Type operator*( vector2<Type> const& l, vector2<Type> const& r )
{
	return l[0]*r[0] + l[1]*r[1];
}

template <typename TypeL,typename TypeR>
vector2<TypeL> operator/( vector2<TypeL> const& l, TypeR val )
{
	return vector2<TypeL>(  l[0] / val, l[1] / val );
}

template <typename Type>
vector2<Type> operator-( vector2<Type> const& l )
{
	return vector2<Type>( - l[0], -l[1]  );
}

template <typename Type>
struct vector3
{
	typedef Type channel_type;
	enum {size = 3 };
	Type data[size];

	static const vector3<Type> null_vector;
	static const vector3<Type> max_vector;
	
	vector3() { data[0] = data[1] = data[2] = 0;}
	vector3( Type a, Type b, Type c) { data[0]=a; data[1]=b; data[2]=c;}
	void set( Type a, Type b, Type c ) { data[0]=a; data[1]=b; data[2]=c;}
	
	Type & operator[]( size_t index ) { return data[index]; }
	Type const& operator[]( size_t index ) const { return data[index]; }

	template <typename TypeR>
	vector3<Type> & operator-=( vector3<TypeR> const& r ) {
		data[0] -= r.data[0];
		data[1] -= r.data[1];
		data[2] -= r.data[2];
		return *this;
	}
	template <typename TypeR>
	vector3<Type> & operator+=( vector3<TypeR> const& r )  {
		data[0] += r.data[0];
		data[1] += r.data[1];
		data[2] += r.data[2];
		return *this;
	}
	template <typename TypeR>
	vector3<Type> & operator*=( TypeR val ) {
		data[0] *= val;
		data[1] *= val;
		data[2] *= val;
		return *this;
	}
	template <typename TypeR>
	vector3<Type> & operator/=( TypeR val ) {
		data[0] /= val;
		data[1] /= val;
		data[2] /= val;
		return *this;
	}

	template <typename Functor>
	vector3<Type> applied( Functor fun ) const 
	{ return vector3<Type>( fun( data[0] ), fun( data[1] ), fun( data[2] ) ); }


	template <typename T>
	bool operator != ( vector3<T> const &r ) const 
	{ return !( this == &r || 
			( r.data[0] == data[0] && 
			  r.data[1] == data[1] &&
			  r.data[2] == data[2] )
			); }

	template <typename T>
	bool operator == ( vector3<T> const &r ) const 
	{ return ( this == &r || 
			( r.data[0] == data[0] && 
			  r.data[1] == data[1] &&
			  r.data[2] == data[2] )
			); }

};

template <typename Type>
const vector3<Type> vector3<Type>::null_vector(0,0,0);

template <typename Type>
const vector3<Type> vector3<Type>::max_vector(
		colour_channel_traits<Type>::max(),
		colour_channel_traits<Type>::max(),
		colour_channel_traits<Type>::max()
		);


template <typename TypeL,typename TypeR>
vector3<TypeL> operator+( vector3<TypeL> const& l, vector3<TypeR> const& r )
{
	return vector3<TypeL>( l[0] + r[0], l[1] + r[1], l[2] + r[2] );
}

template <typename TypeL,typename TypeR>
vector3<TypeL> operator-( vector3<TypeL> const& l, vector3<TypeR> const& r )
{
	return vector3<TypeL>( l[0] - r[0], l[1] - r[1], l[2] - r[2] );
}

template <typename TypeL,typename TypeR>
vector3<TypeL> operator*( vector3<TypeL> const& l, TypeR val )
{
	return vector3<TypeL>( l[0] * val, l[1] * val, l[2] * val );
}

template <typename TypeL,typename TypeR>
inline vector3<TypeR> operator*(TypeL const& val, vector3<TypeR> const& r )
{
	return r*val;
}

template <typename Type>
inline Type operator*( vector3<Type> const& l, vector3<Type> const& r )
{
	return l[0]*r[0] + l[1]*r[1] + l[2]*r[2];
}

template <typename Type>
vector3<Type> operator/( vector3<Type> const& l, Type val )
{
	return vector3<Type>(  l[0] / val, l[1] / val, l[2] / val );
}

template <typename Type>
vector3<Type> operator/( Type val, vector3<Type> const& l )
{
	return vector3<Type>(  val/l[0], val/l[1], val/l[2] );
}

template <typename Type>
vector3<Type> operator-( vector3<Type> const& l )
{
	return vector3<Type>( - l[0], -l[1], -l[2]  );
}

template <typename Type>
struct vector4
{
	typedef Type channel_type;
	enum {size = 4 };
	Type data[size];

	static const vector4<Type> null_vector;
	static const vector4<Type> max_vector;
	
	vector4() { data[0] = data[1] = data[2] = data[3] = 0;}
	vector4( Type a, Type b, Type c, Type d) { data[0]=a; data[1]=b; data[2]=c; data[3]=d;}
	void set( Type a, Type b, Type c, Type d) { data[0]=a; data[1]=b; data[2]=c; data[3]=d;}
	
	Type & operator[]( size_t index ) { return data[index]; }
	Type const& operator[]( size_t index ) const { return data[index]; }

	template <typename TypeR>
	vector4<Type> & operator-=( vector3<TypeR> const& r ) {
		data[0] -= r.data[0];
		data[1] -= r.data[1];
		data[2] -= r.data[2];
		data[3] -= r.data[3];
		return *this;
	}
	template <typename TypeR>
	vector4<Type> & operator+=( vector4<TypeR> const& r )  {
		data[0] += r.data[0];
		data[1] += r.data[1];
		data[2] += r.data[2];
		data[3] += r.data[3];
		return *this;
	}
	template <typename TypeR>
	vector4<Type> & operator*=( TypeR val ) {
		data[0] *= val;
		data[1] *= val;
		data[2] *= val;
		data[3] *= val;
		return *this;
	}
	template <typename TypeR>
	vector4<Type> & operator/=( TypeR val ) {
		data[0] /= val;
		data[1] /= val;
		data[2] /= val;
		data[4] /= val;
		return *this;
	}

	template <typename Functor>
	vector4<Type> applied( Functor fun ) const 
	{ return vector4<Type>( fun( data[0] ), fun( data[1] ), fun( data[2] ), fun( data[3] ) ); }


	template <typename T>
	bool operator != ( vector4<T> const &r ) const 
	{ return !( this == &r || 
			( r.data[0] == data[0] && 
			  r.data[1] == data[1] &&
			  r.data[2] == data[2] &&
			  r.data[3] == data[3] )
			); }

	template <typename T>
	bool operator == ( vector3<T> const &r ) const 
	{ return ( this == &r || 
			( r.data[0] == data[0] && 
			  r.data[1] == data[1] &&
			  r.data[2] == data[2] &&
			  r.data[3] == data[3] )
			); }

};

template <typename Type>
const vector4<Type> vector4<Type>::null_vector(0,0,0);

template <typename Type>
const vector4<Type> vector4<Type>::max_vector(
		colour_channel_traits<Type>::max(),
		colour_channel_traits<Type>::max(),
		colour_channel_traits<Type>::max(),
		colour_channel_traits<Type>::max()
		);


template <typename TypeL,typename TypeR>
vector4<TypeL> operator+( vector4<TypeL> const& l, vector4<TypeR> const& r )
{
	return vector4<TypeL>( l[0] + r[0], l[1] + r[1], l[2] + r[2], l[3] + r[3] );
}

template <typename TypeL,typename TypeR>
vector4<TypeL> operator-( vector4<TypeL> const& l, vector4<TypeR> const& r )
{
	return vector4<TypeL>( l[0] - r[0], l[1] - r[1], l[2] - r[2], l[3] - r[3] );
}

template <typename TypeL,typename TypeR>
vector4<TypeL> operator*( vector4<TypeL> const& l, TypeR val )
{
	return vector4<TypeL>( l[0] * val, l[1] * val, l[2] * val, l[3]*val );
}

template <typename TypeL,typename TypeR>
inline vector4<TypeR> operator*(TypeL const& val, vector4<TypeR> const& r )
{
	return r*val;
}

template <typename Type>
inline Type operator*( vector4<Type> const& l, vector4<Type> const& r )
{
	return l[0]*r[0] + l[1]*r[1] + l[2]*r[2] + l[3]*r[3];
}

template <typename TypeL,typename TypeR>
vector4<TypeL> operator/( vector4<TypeL> const& l, TypeR val )
{
	return vector4<TypeL>(  l[0] / val, l[1] / val, l[2] / val, l[3]/val );
}

template <typename Type>
vector4<Type> operator-( vector4<Type> const& l )
{
	return vector4<Type>( - l[0], -l[1], -l[2], -l[4]);
}

}}
#endif 

