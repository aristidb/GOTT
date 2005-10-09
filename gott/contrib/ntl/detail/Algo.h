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

template <class T>
inline int sgn(T a) { return a > 0 ? 1 : a < 0 ? -1 : 0; }

template <class T>
inline T tabs(T a) { return (a >= 0 ? a : -a); }

template <class T>
inline int cmp(const T& a, const T& b) { return a > b ? 1 : a < b ? -1 : 0; }

template <class I>
void Reverse(I start, I end)
{
	if(start != end && --end != start)
		do
			IterSwap(start, end);
		while(++start != end && --end != start);
}

template <class C>
void Reverse(C& container)
{
	Reverse(container.Begin(), container.End());
}

template <class T, class V>
void Sum(V& sum, T ptr, T end)

{
	while(ptr != end)
		sum += *ptr++;
}

template <class T>
typename T::ValueType Sum(const T& c)
{
	typename T::ValueType sum;
	Sum(sum, c.Begin(), c.End());
	return sum;
}

template <class T>
typename T::ValueType Sum(const T& c, const typename T::ValueType& init)
{
	typename T::ValueType sum = init;
	Sum(sum, c.Begin(), c.End());
	return sum;
}

template <class T>
typename T::ValueType Sum0(const T& c)
{
	typename T::ValueType sum = 0;
	Sum(sum, c.Begin(), c.End());
	return sum;
}

template <class T>
T MinElement(T ptr, T end)
{
	ASSERT(ptr != end);
	T min = ptr;
	while(++ptr != end)
		if(*ptr < *min) min = ptr;
	return min;
}

template <class T>
const typename T::ValueType& Min(const T& c)
{
	return *MinElement(c.Begin(), c.End());
}

template <class T>
T MaxElement(T ptr, T end)
{
	ASSERT(ptr != end);
	T max = ptr;
	while(++ptr != end)
		if(*max < *ptr) max = ptr;
	return max;
}

template <class T>
const typename T::ValueType& Max(const T& c)
{
	return *MaxElement(c.Begin(), c.End());
}

template<class T>
struct StdEqual
{
	bool operator () (const T& a, const T& b) const { return a == b; }
};

template<class T>
struct StdLess {
	bool operator () (const T& a, const T& b) const { return a < b; }
};

template<class T>
struct StdGreater
{
	bool operator () (const T& a, const T& b) const { return a > b; }
};

template <class T, class C>
bool Compare(T ptr1, T end1, T ptr2, T end2, const C& compare)
{
	for(; ptr1 != end1 && ptr2 != end2; ++ptr1, ++ptr2)
		if(!compare(*ptr1, *ptr2)) return false;
	return ptr1 == end1 && ptr2 == end2;
}

template <class T, class C>
bool Compare(const T& c1, const T& c2, const C& compare)
{
	return Compare(c1.Begin(), c1.End(), c2.Begin(), c2.End(), compare);
}

template <class T>
bool Compare(const T& c1, const T& c2)
{
	typedef typename T::ValueType VT;
	return Compare(c1, c2, StdEqual<VT>());
}

template <class T, class V, class C>
T Find(T ptr, T end, const V& value, const C& equal)
{
	while(ptr != end) {
		if(equal(*ptr, value)) return ptr;
		ptr++;
	}
	return NULL;
}

template <class T, class V>
T Find(T ptr, T end, const V& value)
{
	return Find(ptr, end, value, StdEqual<T>());
}

template <class T, class V, class C>
int FindIndex(const T& cont, const V& value, const C& equal)
{
	for(int i = 0; i < cont.GetCount(); i++)
		if(equal(cont[i], value)) return i;
	return -1;
}

template <class T, class V>
int FindIndex(const T& cont, const V& value)
{
	typedef typename T::ValueType VT;
	return FindIndex(cont, value, StdEqual<VT>());
}

//////////////////////////////////////////////////////////////////////
// BinFind

template <class I, class K, class L>
int BinFindIndex(I begin, I end, const K& key, const L& less)
{
	if(begin == end)
		return 0;
	int min = 0;
	int max = end - begin;

	while(min < max)
	{
		int mid = (max + min) >> 1;
		if(less(*(begin + mid), key))
			min = mid + 1;
		else
			max = mid;
	}
	return min;
}

template <class C, class K, class L>
inline int BinFindIndex(const C& container, const K& key, const L& less)
{
	return BinFindIndex(container.Begin(), container.End(), key, less);
}

template <class C, class K>
inline int BinFindIndex(const C& container, const K& key)
{
	typedef typename C::ValueType VT;
	return BinFindIndex(container, key, StdLess<VT>());
}

template <class I, class K, class L>
inline I BinFind(I begin, I end, const K& key, const L& less)
{
	return begin + BinFindIndex(begin, end, key, less);
}

template <class C, class K, class L>
inline typename C::ConstIterator BinFind(const C& container, const K& key, const L& less)
{
	return BinFind(container.Begin(), container.End(), key, less);
}

template <class C, class K>
inline typename C::ConstIterator BinFind(const C& container, const K& key)
{
	typedef typename C::ValueType VT;
	return BinFind(container, key, StdLess<VT>());
}

// -------------------------------------------------------------------

template <class C, class T, class L>
int FindLowerBound(const C& v, int pos, int count, const T& val, const L& less)
{
	while(count > 0) {
		int half = count >> 1;
		int m = pos + half;
		if(less(v[m], val)) {
			pos = m + 1;
			count = count - half - 1;
		}
		else
			count = half;
	}
	return pos;
}

template <class I, class T, class L>
I FindLowerBoundIter(I begin, I end, const T& val, const L& less)
{
	return begin + FindLowerBound(begin, 0, end - begin, val, less);
}

template <class I, class T>
I FindLowerBoundIter(I begin, I end, const T& val)
{
	return begin + FindLowerBound(begin, 0, end - begin, val, StdLess<T>());
}

template <class C, class T, class L>
int FindLowerBound(const C& v, const T& val, const L& less)
{
	return FindLowerBound(v, 0, v.GetCount(), val, less);
}

template <class C, class T>
int FindLowerBound(const C& v, const T& val)
{
	return FindLowerBound(v, val, StdLess<TYPENAME C::ValueType>());
}

template <class C, class T, class L>
int FindUpperBound(const C& v, int pos, int count, const T& val, const L& less)
{
	while(count > 0) {
		int half = count >> 1;
		int m = pos + half;
		if(less(val, v[m]))
			count = half;
		else {
			pos = m + 1;
			count = count - half - 1;
		}
	}
	return pos;
}

template <class I, class T, class L>
I FindUpperBoundIter(I begin, I end, const T& val, const L& less)
{
	return begin + FindUpperBound(begin, 0, end - begin, val, less);
}

template <class I, class T>
I FindUpperBoundIter(I begin, I end, const T& val)
{
	return begin + FindUpperBound(begin, 0, end - begin, val, StdLess<T>());
}

template <class C, class T, class L>
int FindUpperBound(const C& v, const T& val, const L& less)
{
	return FindUpperBound(v, 0, v.GetCount(), val, less);
}

template <class C, class T>
int FindUpperBound(const C& v, const T& val)
{
	return FindUpperBound(v, val, StdLess<TYPENAME C::ValueType>());
}

template <class C, class T, class L>
int FindBinary(const C& v, const T& val, int pos, int count, const L& less)
{
	int i = FindLowerBound(v, pos, count, val, less);
	return i < count && !less(val, v[i]) ? i : -1;
}

template <class I, class T, class L>
I FindBinaryIter(I begin, I end, const T& val, const L& less)
{
	int q = FindUpperBound(begin, begin, end, val, less);
	return q < 0 ? NULL : begin + q;
}

template <class I, class T>
I FindBinaryIter(I begin, I end, const T& val)
{
	return FindBinaryIter(begin, end, val, StdLess<T>());
}

template <class C, class T, class L>
int FindBinary(const C& v, const T& val, const L& less)
{
	return FindBinary(v, val, 0, v.GetCount(), less);
}

template <class C, class T>
int FindBinary(const C& v, const T& val)
{
	return FindBinary(v, val, StdLess<TYPENAME C::ValueType>());
}

//////////////////////////////////////////////////////////////////////
// BinFindComp

template <class I, class K, class X>
int BinFindCompIndex(I begin, I end, const K& key, const X& comp)
{
	if(begin == end) // empty array
		return 0;
	int min = 0;
	int max = end - begin;
	while(min < max)
	{
		int mid = (max + min) >> 1;
		if(comp.Compare(key, *(begin + mid)) > 0)
			min = mid + 1;
		else
			max = mid;
	}
	return min;
}

template <class C, class K, class X>
inline int BinFindCompIndex(const C& container, const K& key, const X& comp)
{
	return BinFindCompIndex(container.Begin(), container.End(), key, comp);
}

template <class I, class K, class X>
inline I BinFindComp(I begin, I end, const K& key, const X& comp)
{
	return begin + BinFindCompIndex(begin, end, key, comp);
}

template <class C, class K, class X>
inline typename C::ConstIterator BinFindComp(const C& container, const K& key, const X& comp)
{
	return BinFindComp(container.Begin(), container.End(), key, comp);
}

//////////////////////////////////////////////////////////////////////
// Append

template <class T, class V>
void Append(T& dst, V ptr, V end)
{
	for(; ptr != end; ++ptr)
		dst.Add(*ptr);
}

template <class T, class V>
void Append(T& dst, V ptr, int n)
{
	for(; n--; ++ptr)
		dst.Add(*ptr);
}

template <class T, class V>
void Append(T& dst, const V& src)
{
	Append(dst, src.Begin(), src.End());
}

//////////////////////////////////////////////////////////////////////
// FindAppend::

template <class C, class I>
C& FindAppend(C& dest, I begin, I end)
{
	for(; begin != end; ++begin)
		dest.FindAdd(*begin);
	return dest;
}

//////////////////////////////////////////////////////////////////////

template <class C, class S>
inline C& FindAppend(C& dest, const S& source)
{
	return FindAppend(dest, source.Begin(), source.End());
}

//////////////////////////////////////////////////////////////////////

template <class C, class L>
C& AppendSorted(C& dest, const C& src, const L& less)
{
	if(src.IsEmpty())
		return dest;
	if(dest.IsEmpty())
	{
		dest <<= src;
		return dest;
	}
	if(!less(*src, dest.Top()))
	{
		dest.Append(src);
		return dest;
	}
	if(!less(*dest, src.Top()))
	{
		dest.Insert(0, src);
		return dest;
	}
	int dc = dest.GetCount();
	int sc = src.GetCount();
	dest.SetCount(dc + sc);
	typename C::Iterator de = dest.End();
	typename C::ConstIterator se = src.End(), pe = dest.GetIter(dc);
	--se;
	--pe;
	for(;;)
	{
		if(less(*se, *pe))
		{
			*--de = *pe;
			if(pe == dest.Begin())
			{ // dest items are finished
				*--de = *se;
				while(se != src.Begin())
					*--de = *--se;
				return dest;
			}
			--pe;
		}
		else
		{
			*--de = *se;
			if(se == src.Begin())
				return dest; // src items are finished, dest items are in place
			--se;
		}
	}
	return dest;
}

//////////////////////////////////////////////////////////////////////

template <class C>
C& AppendSorted(C& dest, const C& src)
{
	typedef typename C::ValueType VT;
	return AppendSorted(dest, src, StdLess<VT>());
}

//////////////////////////////////////////////////////////////////////

template <class C, class L>
C& UnionSorted(C& dest, const C& src, const L& less)
{
	if(src.IsEmpty())
		return dest;
	if(dest.IsEmpty())
	{
		dest <<= src;
		return dest;
	}
	if(less(dest.Top(), *src))
	{
		dest.Append(src);
		return dest;
	}
	if(less(src.Top(), *dest))
	{
		dest.Insert(0, src);
		return dest;
	}
	int dc = dest.GetCount();
	int sc = src.GetCount();
	dest.SetCount(dc + sc);
	typename C::Iterator de = dest.End();
	typename C::ConstIterator se = src.End(), pe = dest.GetIter(dc);
	--se;
	--pe;
	for(;;)
	{
		if(less(*se, *pe))
		{
			*--de = *pe;
			if(pe == dest.Begin())
			{ // dest items are finished
				*--de = *se;
				while(se != src.Begin())
					*--de = *--se;
				dest.Remove(0, dest.GetIndex(*de));
				return dest;
			}
			--pe;
		}
		else
		{
			*--de = *se;
			if(!less(*pe, *se))
			{
				if(pe == dest.Begin())
				{
					while(se != src.Begin())
						*--de = *--se;
					dest.Remove(0, dest.GetIndex(*de));
					return dest;
				}
				--pe;
			}
			if(se == src.Begin())
			{
				int pi = (pe - dest.Begin()) + 1;
				dest.Remove(pi, (de - dest.Begin()) - pi);
				return dest; // src items are finished, dest items are in place
			}
			--se;
		}
	}
	return dest;
}

//////////////////////////////////////////////////////////////////////

template <class C>
C& UnionSorted(C& dest, const C& src)
{
	typedef typename C::ValueType VT;
	return UnionSorted(dest, src, StdLess<VT>());
}

//////////////////////////////////////////////////////////////////////

template <class C, class L>
C& RemoveSorted(C& from, const C& what, const L& less)
{
	if(from.IsEmpty() || what.IsEmpty() ||
	   less(from.Top(), *what.Begin()) || less(what.Top(), *from.Begin()))
		return from;
	typename C::ConstIterator we = what.End(), wp = BinFind(what, from[0], less);
	if(wp == we)
		return from;
	typename C::Iterator fp = from.Begin() + BinFindIndex(from, *wp), fe = from.End(), fd = fp;
	if(fp == fe)
	{
		from.Clear();
		return from;
	}
	for(;;)
	{
		while(less(*fp, *wp))
		{
			*fd = *fp;
			++fd;
			if(++fp == fe)
			{
				from.SetCount(fd - from.Begin());
				return from;
			}
		}
		if(less(*wp, *fp))
		{
			do
				if(++wp == we)
				{
					Copy(fd, fp, fe);
					fd += (fe - fp);
					from.SetCount(fd - from.Begin());
					return from;
				}
			while(less(*wp, *fp));
		}
		else
		{
			const typename C::ValueType& value = *fp;
			while(!less(value, *fp))
				if(++fp == fe)
				{
					from.SetCount(fd - from.Begin());
					return from;
				}
			do
				if(++wp == we)
				{
					Copy(fd, fp, fe);
					fd += (fe - fp);
					from.SetCount(fd - from.Begin());
					return from;
				}
			while(!less(value, *wp));
		}
	}
}

//////////////////////////////////////////////////////////////////////

template <class C>
C& RemoveSorted(C& from, const C& what)
{
	typedef typename C::ValueType VT;
	return RemoveSorted(from, what, StdLess<VT>());
}

//////////////////////////////////////////////////////////////////////

template <class D, class S, class L>
D& IntersectSorted(D& dest, const S& src, const L& less)
{
	if(dest.IsEmpty())
		return dest;
	if(src.IsEmpty() || less(dest.Top(), src[0]) || less(src.Top(), dest[0]))
	{ // empty source set or disjunct intervals
		dest.Clear();
		return dest;
	}
	typename S::ConstIterator ss = BinFind(src, dest[0], less), se = src.End();
	if(ss == se)
	{
		dest.Clear();
		return dest;
	}
	typename D::ConstIterator ds = BinFind(dest, src[0], less), de = dest.End();
	if(ds == de)
	{
		dest.Clear();
		return dest;
	}
	typename D::Iterator d = dest.Begin();
	int count = 0;
	for(;;)
	{
		if(less(*ss, *ds))
		{
			if(++ss == se)
				break;
		}
		else
		{
			if(!less(*ds, *ss))
			{
				*d = *ds;
				++d;
				count++;
			}
			if(++ds == de)
				break;
		}
	}
	dest.SetCount(count);
	return dest;
}

//////////////////////////////////////////////////////////////////////

template <class D, class S>
D& IntersectSorted(D& dest, const S& src)
{
	typedef typename D::ValueType VT;
	return IntersectSorted(dest, src, StdLess<VT>());
}

//////////////////////////////////////////////////////////////////////
// StreamContainer

#ifdef UPP
template <class T>
void StreamContainer(Stream& s, T& cont)
{
	int n = cont.GetCount();
	s / n;
	if(s.IsLoading())
	{
		cont.Clear();
		while(n--)
			s % cont.Add();
	}
	else
	{
		for(typename T::Iterator ptr = cont.Begin(); n--; ++ptr)
			s % *ptr;
	}
}
#endif
//////////////////////////////////////////////////////////////////////
// ForwardSort

template <class I, class Less>
void ForwardSort(I begin, I end, const Less& less)
{
	if(begin == end)
		return;
	I limit = end;
	--limit;
	while(!(begin == limit))
	{
		for(I best = limit, next = limit, ptr = limit;; best = ptr)
			if(!less(*best, *--ptr))
			{
				if(ptr == begin)
				{
					begin = next;
					break;
				}
			}
			else
			{
				do
				{
					if(ptr == begin)
					{
						IterSwap(begin, best);
						++begin;
						goto NEXT_ITEM;
					}
				}
				while(less(*best, *--ptr));
				if(ptr == begin)
				{
					IterSwap(++begin, best);
					++begin;
					break;
				}
				next = ptr;
				++next;
			}
	NEXT_ITEM:
		;
	}
}

//////////////////////////////////////////////////////////////////////

template <class T, class Less>
void ForwardSort(T& c, const Less& less)
{
	ForwardSort(c.Begin(), c.End(), less);
}

//////////////////////////////////////////////////////////////////////

template <class T>
void ForwardSort(T& c)
{
	typedef typename T::ValueType VT;
	ForwardSort(c.Begin(), c.End(), StdLess<VT>());
}

//////////////////////////////////////////////////////////////////////

enum
{
	__SORT_THRESHOLD = 16
};

//////////////////////////////////////////////////////////////////////
// Sort

template <class I, class Less>
inline I IterMedian(I x, I y, I z, const Less& less)
{
	return less(*x, *y)
		? less(*y, *z) ? y : less(*x, *z) ? z : x
		: less(*x, *z) ? x : less(*y, *z) ? z : y;
}

template <class I, class Less>
void Sort(I begin, I end, const Less& less)
{
	int count;
	while((count = end - begin) > __SORT_THRESHOLD)
	{
		I b = begin, e = end, m = IterMedian(b, b + (count >> 1), e - 1, less);
		for(;; ++b)
		{
			while(less(*m, *--e))
				;
			while(less(*b, *m))
				++b;
			if(!(b < e))
				break;
			if(m == b) m = e;
			else if(m == e) m = b;
			IterSwap(b, e);
		}
		if(b - begin < end - e)
		{
			Sort(begin, b, less);
			begin = b;
		}
		else
		{
			Sort(b, end, less);
			end = b;
		}
	}
	if(count >= 1)
		ForwardSort(begin, end, less);
}

template <class T, class Less>
void Sort(T& c, int l, int h, const Less& less)
{
	Sort(c.GetIter(l), c.GetIter(h), less);
}

template <class T, class Less>
void Sort(T& c, const Less& less)
{
	Sort(c.Begin(), c.End(), less);
}

template <class T>
void Sort(T& c)
{
	typedef typename T::ValueType VT;
	Sort(c.Begin(), c.End(), StdLess<VT>());
}

//////////////////////////////////////////////////////////////////////
// IndexSort

template <class II, class VI, class K>
struct IndexSortIterator
{
	typedef IndexSortIterator<II, VI, K> Iter;

	IndexSortIterator(II ii, VI vi) : ii(ii), vi(vi) {}

	Iter&       operator ++ ()               { ++ii; ++vi; return *this; }
	Iter&       operator -- ()               { --ii; --vi; return *this; }
	const K&    operator *  () /*const*/     { return *ii; } //!! Fidler's bug in Array::Iterator
	Iter        operator +  (int i) const    { return Iter(ii + i, vi + i); }
	Iter        operator -  (int i) const    { return Iter(ii - i, vi - i); }
	int         operator -  (Iter b) const   { return (int)(ii - b.ii); }
	bool        operator == (Iter b) const   { return ii == b.ii; }
	bool        operator <  (Iter b) const   { return ii <  b.ii; }
	friend void IterSwap    (Iter a, Iter b) { IterSwap(a.ii, b.ii); IterSwap(a.vi, b.vi); }

	II          ii;
	VI          vi;
};

template <class II, class VI, class K, class Less>
inline void __IndexSort(II begin, II end, VI pair, const Less& less, const K *)
{
	Sort(IndexSortIterator<II, VI, K>(begin, pair),
		IndexSortIterator<II, VI, K>(end, pair + (end - begin)),
		less);
}

//////////////////////////////////////////////////////////////////////

template <class II, class VI, class Less>
inline void IndexSort(II begin, II end, VI pair, const Less& less)
{
	if(begin != end)
		__IndexSort(begin, end, pair, less, &*begin);
}

//////////////////////////////////////////////////////////////////////

template <class KC, class VC, class Less>
inline void IndexSort(KC& keys, VC& values, const Less& less)
{
	typedef typename KC::ValueType KT;
	ASSERT(keys.GetCount() == values.GetCount());
	if(keys.GetCount() >= 2)
		__IndexSort(keys.Begin(), keys.End(), values.Begin(), less, (KT *)0);
}

//////////////////////////////////////////////////////////////////////

template <class KC, class VC>
inline void IndexSort(KC& keys, VC& values)
{
	typedef typename KC::ValueType KT;
	if(keys.GetCount() >= 2)
		__IndexSort(keys.Begin(), keys.End(), values.Begin(), StdLess<KT>(), (KT *)0);
}

//////////////////////////////////////////////////////////////////////

template <class I, class V>
struct SortOrderIterator
{
	typedef SortOrderIterator<I, V> Iter;

	SortOrderIterator(int *ii, I vi) : ii(ii), vi(vi) {}

	Iter&       operator ++ ()               { ++ii; return *this; }
	Iter&       operator -- ()               { --ii; return *this; }
	const V&    operator *  () const         { return *(vi + *ii); }
	Iter        operator +  (int i) const    { return Iter(ii + i, vi); }
	Iter        operator -  (int i) const    { return Iter(ii - i, vi); }
	int         operator -  (Iter b) const   { return int(ii - b.ii); }
	bool        operator == (Iter b) const   { return ii == b.ii; }
	bool        operator <  (Iter b) const   { return ii < b.ii; }
	friend void IterSwap    (Iter a, Iter b) { IterSwap(a.ii, b.ii); }

	int        *ii;
	I           vi;
};

template <class I, class V, class Less>
inline void __SortOrder(int *begin, int *end, I data, const Less& less, const V *)
{
	Sort(SortOrderIterator<I, V>(begin, data), SortOrderIterator<I, V>(end, data), less);
}

//////////////////////////////////////////////////////////////////////

template <class I, class Less>
inline Vector<int> GetSortOrder(I begin, I end, const Less& less)
{
	Vector<int> index;
	index.SetCount((int)(end - begin));
	for(int i = index.GetCount(); --i >= 0; index[i] = i)
		;
	__SortOrder(index.Begin(), index.End(), begin, less, &*begin);
	return index;
}

//////////////////////////////////////////////////////////////////////

template <class C, class Less>
inline Vector<int> GetSortOrder(const C& container, const Less& less)
{
	return GetSortOrder(container.Begin(), container.End(), less);
}

//////////////////////////////////////////////////////////////////////

template <class C>
inline Vector<int> GetSortOrder(const C& container)
{
	typedef typename C::ValueType V;
	return GetSortOrder(container.Begin(), container.End(), StdLess<V>());
}

//////////////////////////////////////////////////////////////////////

template <class DC, class I, class F>
void GetFieldContainer(DC& dest, I begin, I end, F field)
{
	for(; begin != end; ++begin)
		dest.Add((*begin).*field);
}

//////////////////////////////////////////////////////////////////////

template <class DC, class SC, class F>
void GetFieldContainer(DC& dest, const SC& src, F field)
{ GetFieldContainer<DC, TYPENAME SC::ConstIterator, F>(dest, src.Begin(), src.End(), field); }

//////////////////////////////////////////////////////////////////////

template <class I, class F, class O, class E>
I FindField(I begin, I end, F field, const O& object, const E& equal)
{
	for(; begin != end && !equal((*begin).*field, object); ++begin)
		;
	return begin;
}

//////////////////////////////////////////////////////////////////////

template <class I, class F, class O>
I FindField(I begin, I end, F field, const O& object)
{ return FindField(begin, end, field, object, StdEqual<O>()); }

//////////////////////////////////////////////////////////////////////

template <class C, class F, class O, class E>
int FindFieldIndex(const C& container, F field, const O& object, const E& equal)
{
	int i = 0;
	for(typename C::ConstIterator b = container.Begin(), e = container.End(); b != e; ++b, ++i)
		if(equal((*b).*field, object))
			return i;
	return -1;
}

//////////////////////////////////////////////////////////////////////

template <class C, class F, class O>
int FindFieldIndex(const C& container, F field, const O& object)
{ return FindFieldIndex(container, field, object, StdEqual<O>()); }

//////////////////////////////////////////////////////////////////////

template <class O, class T, class R>
class FieldRelationCls {
	O        T::*member;
	const R& relation;

public:
	FieldRelationCls(O (T::*member), const R& relation) : member(member), relation(relation) {}
	bool operator () (const T& t1, const T& t2) const { return relation(t1.*member, t2.*member); }
};

template <class O, class T, class R>
inline FieldRelationCls<O, T, R> FieldRelation(O (T::*member), const R& relation)
{ return FieldRelationCls<O, T, R>(member, relation); }

//////////////////////////////////////////////////////////////////////

template <class M, class T, class R>
class MethodRelationCls {
	M        method;
	const R& relation;

public:
	MethodRelationCls(M method, const R& relation) : method(method), relation(relation) {}
	bool operator () (const T& t1, const T& t2) const {
		return relation((t1.*method)(), (t2.*method)());
	}
};

template <class O, class T, class R>
inline MethodRelationCls<O (T::*)(), T, R>
	MethodRelation(O (T::*method)(), const R& relation)
{ return MethodRelationCls<O (T::*)(), T, R>(method, relation); }

template <class O, class T, class R>
inline MethodRelationCls<O (T::*)() const, T, R>
	MethodRelation(O (T::*method)() const, const R& relation)
{ return MethodRelationCls<O (T::*)() const, T, R>(method, relation); }

///////////////////////////////////////////////////////////////////////

template <class C, class T>
void LruAdd(C& lru, T value, int limit = 10) {
	int q = FindIndex(lru, value);
	if(q >= 0)
		lru.Remove(q);
	lru.Insert(0, value);
	if(lru.GetCount() > limit)
		lru.SetCount(limit);
}
