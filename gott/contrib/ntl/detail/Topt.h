////////////////////////////////////////////////////////////////////////////////
// NTL Nonstandard Template Library for C++
// Copyright (c) 2003 by Miroslav Fidler, Tomas Rylek
//
// Created by Miroslav Fidler, Tomas Rylek, cxl@volny.cz
//
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this software 
//     for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Changes (C) 2005 Aristid Breitkreuz, LGPL 2.1

#include <gott/util/visibility.hpp>

template <class T>
inline void Swap(T& a, T& b) { T tmp = a; a = b; b = tmp; }

template <class I>
inline void IterSwap(I a, I b) { if(a != b) Swap(*a, *b); }

template <class T>
class RelOps
{
	friend bool operator >  (const T& a, const T& b)  { return b < a; }
	friend bool operator != (const T& a, const T& b)  { return !(a == b); }
	friend bool operator <= (const T& a, const T& b)  { return !(b < a); }
	friend bool operator >= (const T& a, const T& b)  { return !(a < b); }
	
	void operator=(const RelOps&) {} // MSC 6.0 empty base class bug fix
};

template <class U, class V>
class AddOps
{
	friend U& operator -= (U& a, const V& b)          { a += -b; return a; }
	friend U  operator +  (const U& a, const V& b)    { U x(a); x += b; return x; }
	friend U  operator -  (const U& a, const V& b)    { U x(a); x += -b; return x; }

	void operator=(const AddOps&) {} // MSC 6.0 empty base class bug fix
};

template <class T>
class PostfixOps
{
	friend T operator ++ (T& i, int)                  { T x = i; ++i; return x; }
	friend T operator -- (T& i, int)                  { T x = i; --i; return x; }

	void operator=(const PostfixOps&) {} // MSC 6.0 empty base class bug fix
};

template <class T, int (*compare)(T a, T b)>
class CompareRelOps
{
	friend bool operator <  (T a, T b)                { return (*compare)(a, b) <  0; }
	friend bool operator >  (T a, T b)                { return (*compare)(a, b) >  0; }
	friend bool operator == (T a, T b)                { return (*compare)(a, b) == 0; }
	friend bool operator != (T a, T b)                { return (*compare)(a, b) != 0; }
	friend bool operator <= (T a, T b)                { return (*compare)(a, b) <= 0; }
	friend bool operator >= (T a, T b)                { return (*compare)(a, b) >= 0; }

	void operator=(const CompareRelOps&) {} // MSC 6.0 empty base class bug fix
};

template <class T>
inline void Fill(T *dst, const T *lim, const T& x) {
	while(dst < lim)
		*dst++ = x;
}

template <class T>
inline void Copy(T *dst, const T *src, const T *lim) {
	while(src < lim)
		*dst++ = *src++;
}

template <class T>
inline void Copy(T *dst, const T *src, int n) {
	Copy(dst, src, src + n);
}

inline void Fill(char *t, const char *lim, const char& x)
{ memset(t, x, size_t(lim - t)); }
inline void Copy(char *dst, const char *src, const char *lim)
{ memcpy(dst, src, size_t((byte *)lim - (byte *)src)); }

inline void Fill(signed char *t, const signed char *lim, const signed char& x)
{ memset(t, x, size_t(lim - t)); }
inline void Copy(signed char *dst, const signed char *src, const signed char *lim)
{ memcpy(dst, src, size_t((byte *)lim - (byte *)src)); }

inline void Fill(unsigned char *t, const unsigned char *lim, const unsigned char& x)
{ memset(t, x, size_t(lim - t)); }
inline void Copy(unsigned char *dst, const unsigned char *src, const unsigned char *lim)
{ memcpy(dst, src, size_t((byte *)lim - (byte *)src)); }

template <class T>
inline void DeepCopyConstruct(void *p, const T& x) {
	::new(p) T(x);
}

template <class T>
inline T *DeepCopyNew(const T& x) {
	return new T(x);
}

template <class T>
inline void ConstructArray(T *t, const T *lim) {
	while(t < lim)
		::new(t++) T;
}

template <class T>
inline void DestroyArray(T *t, const T *lim) {
	while(t < lim) {
		t->T::~T();
		t++;
	}
}

template <class T>
inline void DeepCopyConstructArray(T *t, const T *s, const T *lim) {
	while(s < lim)
		DeepCopyConstruct(t++, *s++);
}

template <class T>
inline void DeepCopyConstructFill(T *t, const T *lim, const T& x) {
	while(t < lim)
		DeepCopyConstruct(t++, x);
}

#ifdef NO_MOVEABLE_CHECK

template <class T>
inline void AssertMoveable() {}

#define MoveableTemplate(T)

template <class T>
class Moveable
{
	void operator=(const Moveable&) {} // MSC 6.0 empty base class bug fix
};

#define NTL_MOVEABLE(T)

#else

template <class T>
inline void AssertMoveablePtr(T, T) {}

template <class T>
inline void AssertMoveable0(T *t) { AssertMoveablePtr(&**t, *t); }

template <class T>
inline void AssertMoveable(T *t = 0) { if(t) AssertMoveable0(t); }

template <class T>
struct Moveable {
	friend void AssertMoveable0(T *) {}
	void operator=(const Moveable&) {} // MSC 6.0 empty base class bug fix
};

#if defined(COMPILER_MSC) || defined(COMPILER_GNU)
	#define NTL_MOVEABLE(T) inline void AssertMoveable0(T *) {}
#else
	#define NTL_MOVEABLE(T) template<> inline void AssertMoveable<T>(T *) {}
#endif

#endif

NTL_MOVEABLE(bool);
NTL_MOVEABLE(char);
NTL_MOVEABLE(signed char);
NTL_MOVEABLE(unsigned char);
NTL_MOVEABLE(short);
NTL_MOVEABLE(unsigned short);
NTL_MOVEABLE(int);
NTL_MOVEABLE(unsigned int);
NTL_MOVEABLE(long);
NTL_MOVEABLE(unsigned long);
NTL_MOVEABLE(float);
NTL_MOVEABLE(double);
NTL_MOVEABLE(void *);
NTL_MOVEABLE(const void *);

template <class T>
class DeepCopyOption {
public:
	friend T& operator<<=(T& dest, const T& src)
	{ (&dest)->T::~T(); ::new(&dest) T(src, 1); return dest; }
	friend void DeepCopyConstruct(void *dest, const T& src)
	{ ::new (dest) T(src, 0); }
	friend T *DeepCopyNew(const T& src)
	{ return ::new T(src, 0); }
	void operator=(const DeepCopyOption&) {} // MSC 6.0 empty base class bug fix
};

template <class T>
class PolyDeepCopyNew
{
public:
	friend T *DeepCopyNew(const T& t)              { return t.Copy(); }
	void operator=(const PolyDeepCopyNew&) {} // MSC 6.0 empty base class bug fix
};

template <class T>
class WithDeepCopy : public T {
public:
	WithDeepCopy(const T& a) : T(a, 1)             {}
	WithDeepCopy(const WithDeepCopy& a) : T(a, 1)  {}
	WithDeepCopy& operator=(const WithDeepCopy& a) { (T&)*this <<= a; return *this; }
	WithDeepCopy(int, pick_ T& a) : T(a)           {}
	WithDeepCopy& operator^=(pick_ T& a)           { (T&)*this = a; return *this; }
	WithDeepCopy()                                 {}
};

template <class V>
class ConstIIterator {
protected:
	const V       *cont;
	int            ii;
	typedef        TYPENAME V::ValueType T;
	struct NP { int dummy; };

public:
	const T&       operator*() const       { return (*cont)[ii]; }
	const T       *operator->() const      { return &(*cont)[ii]; }
	const T&       operator[](int i) const { return (*cont)[ii + i]; }

	ConstIIterator& operator++()           { ++ii; return *this; }
	ConstIIterator& operator--()           { --ii; return *this; }
	ConstIIterator  operator++(int)        { ConstIIterator t = *this; ++ii; return t; }
	ConstIIterator  operator--(int)        { ConstIIterator t = *this; --ii; return t; }

	ConstIIterator& operator+=(int d)      { ii += d; return *this; }
	ConstIIterator& operator-=(int d)      { ii -= d; return *this; }

	ConstIIterator  operator+(int d) const { return ConstIIterator(cont, ii + d); }
	ConstIIterator  operator-(int d) const { return ConstIIterator(cont, ii - d); }

	int  operator-(const ConstIIterator& b) const   { return ii - b.ii; }

	bool operator==(const ConstIIterator& b) const  { return ii == b.ii; }
	bool operator!=(const ConstIIterator& b) const  { return ii != b.ii; }
	bool operator<(const ConstIIterator& b) const   { return ii < b.ii; }
	bool operator>(const ConstIIterator& b) const   { return ii > b.ii; }
	bool operator<=(const ConstIIterator& b) const  { return ii <= b.ii; }
	bool operator>=(const ConstIIterator& b) const  { return ii >= b.ii; }

	operator bool() const     { return ii < 0; }

	ConstIIterator()          {}
	ConstIIterator(NP *null)  { ASSERT(null == NULL); ii = -1; }
	ConstIIterator(const V& _cont, int ii) : cont(&_cont), ii(ii) {}
};

template <class V>
class IIterator {
protected:
	V             *cont;
	int            ii;
	typedef        TYPENAME V::ValueType T;
	struct NP { int dummy; };

public:
	T&       operator*()              { return (*cont)[ii]; }
	T       *operator->()             { return &(*cont)[ii]; }
	T&       operator[](int i)        { return (*cont)[ii + i]; }

	const T& operator*() const        { return (*cont)[ii]; }
	const T *operator->() const       { return &(*cont)[ii]; }
	const T& operator[](int i) const  { return (*cont)[ii + i]; }

	IIterator& operator++()           { ++ii; return *this; }
	IIterator& operator--()           { --ii; return *this; }
	IIterator  operator++(int)        { IIterator t = *this; ++ii; return t; }
	IIterator  operator--(int)        { IIterator t = *this; --ii; return t; }

	IIterator& operator+=(int d)      { ii += d; return *this; }
	IIterator& operator-=(int d)      { ii -= d; return *this; }

	IIterator  operator+(int d) const { return IIterator(*cont, ii + d); }
	IIterator  operator-(int d) const { return IIterator(*cont, ii - d); }

	int  operator-(const IIterator& b) const   { return ii - b.ii; }

	bool operator==(const IIterator& b) const  { return ii == b.ii; }
	bool operator!=(const IIterator& b) const  { return ii != b.ii; }
	bool operator<(const IIterator& b) const   { return ii < b.ii; }
	bool operator>(const IIterator& b) const   { return ii > b.ii; }
	bool operator<=(const IIterator& b) const  { return ii <= b.ii; }
	bool operator>=(const IIterator& b) const  { return ii >= b.ii; }

	operator bool() const                      { return ii < 0; }

	IIterator()          {}
	IIterator(NP *null)  { ASSERT(null == NULL); ii = -1; }
	IIterator(V& _cont, int ii) : cont(&_cont), ii(ii) {}
};

GOTT_EXPORT int Pow2Bound(int i);

GOTT_EXPORT unsigned memhash(const void *ptr, size_t size);

inline unsigned GetHashValue0(const void *ptr)  { return (unsigned)(size_t) ptr; }
inline unsigned GetHashValue0(char a)           { return (unsigned)a; }
inline unsigned GetHashValue0(signed char a)    { return (unsigned)a; }
inline unsigned GetHashValue0(unsigned char a)  { return (unsigned)a; }
inline unsigned GetHashValue0(short a)          { return (unsigned)a; }
inline unsigned GetHashValue0(unsigned short a) { return (unsigned)a; }
inline unsigned GetHashValue0(int a)            { return (unsigned)a; }
inline unsigned GetHashValue0(unsigned int a)   { return (unsigned)a; }
inline unsigned GetHashValue0(long a)           { return (unsigned)a; }
inline unsigned GetHashValue0(unsigned long a)  { return (unsigned)a; }
inline unsigned GetHashValue0(bool a)           { return (unsigned)a; }

GOTT_EXPORT unsigned GetHashValue0(const double& a);

template <class T>
inline unsigned GetHashValue(const T& ptr) { return GetHashValue0(ptr); }

struct CombineHash {
	unsigned hash;

public:
	CombineHash& Put(unsigned h) { hash = ((hash << 4) + hash) ^ h; return *this;}

	operator unsigned() const    { return hash; }

	CombineHash()                              { hash = 1234567890U; }
	CombineHash(unsigned h1)                   { hash = 1234567890U; Put(h1); }
	CombineHash(unsigned h1, unsigned h2)      { hash = 1234567890U; Put(h1); Put(h2); }
	CombineHash(unsigned h1, unsigned h2, unsigned h3)
	{ hash = 1234567890U; Put(h1); Put(h2); Put(h3); }
	CombineHash(unsigned h1, unsigned h2, unsigned h3, unsigned h4)
	{ hash = 1234567890U; Put(h1); Put(h2); Put(h3); Put(h4); }
};

// Allocation hack. In UPP, there is no need for this, but using NTL outside might benefit
// warning - currently there is a bug that results in invalid count/size parameters
// dont use them before fixed

#ifndef NTL_RAW_ALLOC

#define NTL_RAW_ALLOC(size)               new byte[(size)]
#define NTL_RAW_FREE(ptr, count, size)    delete[] (byte *)(ptr)

#endif

// workaround for broken standard libraries...

template <class T> inline const T& ntl_max(const T& a, const T& b) { return a > b ? a : b; }

// STL compatibility hacks

#define STL_INDEX_COMPATIBILITY(C) \
	typedef T             value_type; \
	typedef ConstIterator const_iterator; \
	typedef const T&      const_reference; \
	typedef int           size_type; \
	typedef int           difference_type; \
	const_iterator        begin() const          { return B::Begin(); } \
	const_iterator        end() const            { return B::End(); } \
	void                  clear()                { B::Clear(); } \
	size_type             size()                 { return B::GetCount(); } \

#define STL_BI_COMPATIBILITY(C) \
	typedef T             value_type; \
	typedef ConstIterator const_iterator; \
	typedef const T&      const_reference; \
	typedef int           size_type; \
	typedef int           difference_type; \
	const_iterator        begin() const          { return Begin(); } \
	const_iterator        end() const            { return End(); } \
	void                  clear()                { Clear(); } \
	size_type             size()                 { return GetCount(); } \
	typedef Iterator      iterator; \
	typedef T&            reference; \
	iterator              begin()                { return Begin(); } \
	iterator              end()                  { return End(); } \

#define STL_MAP_COMPATIBILITY(C) \
	typedef T             value_type; \
	typedef ConstIterator const_iterator; \
	typedef const T&      const_reference; \
	typedef int           size_type; \
	typedef int           difference_type; \
	const_iterator        begin() const          { return B::Begin(); } \
	const_iterator        end() const            { return B::End(); } \
	void                  clear()                { B::Clear(); } \
	size_type             size()                 { return B::GetCount(); } \
	typedef Iterator      iterator; \
	typedef T&            reference; \
	iterator              begin()                { return B::Begin(); } \
	iterator              end()                  { return B::End(); } \

#define STL_VECTOR_COMPATIBILITY(C) \
	typedef T             value_type; \
	typedef ConstIterator const_iterator; \
	typedef const T&      const_reference; \
	typedef int           size_type; \
	typedef int           difference_type; \
	const_iterator        begin() const          { return Begin(); } \
	const_iterator        end() const            { return End(); } \
	void                  clear()                { Clear(); } \
	size_type             size()                 { return GetCount(); } \
	typedef Iterator      iterator; \
	typedef T&            reference; \
	iterator              begin()                { return Begin(); } \
	iterator              end()                  { return End(); } \
	reference             front()                { return (*this)[0]; } \
	const_reference       front() const          { return (*this)[0]; } \
	reference             back()                 { return Top(); } \
	const_reference       back() const           { return Top(); } \
	void                  push_back(const T& x)  { Add(x); } \
	void                  pop_back()             { Drop(); }
