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

template <int size>
void RawVector<size>::Realloc(int newalloc)
{
	ASSERT(newalloc >= items);
	Chk();
	if(newalloc == alloc) return;
	Data *newvector = newalloc ? Alloc(newalloc) : NULL;
	if(vector) {
		memcpy(newvector, vector, items * sizeof(Data));
		RawFree();
	}
	vector = newvector;
	alloc = newalloc;
}

template <int size>
void RawVector<size>::Expand()
{
#ifdef _DEBUG
	Realloc(ntl_max(2 * alloc, 1));
#else
	Realloc(ntl_max(2 * alloc, size == 4 ? 8 : 1));
#endif
}

template <int size>
void RawVector<size>::RawFree()
{
	NTL_RAW_FREE(vector, size, sizeof(T));
}

template <int size>
void RawVector<size>::Pick(pick_ RawVector& v)
{
	vector = v.vector;
	items = v.items;
	alloc = v.alloc;
	SetPicked(v);
}

template <int size>
int  RawVector<size>::RawGetIndex(const void *item) const
{
	Chk();
	if(vector == NULL) return -1;
	int n = (const Data *)item - vector;
	return n >= 0 && n < items ? n : -1;
}

template <int size>
void RawVector<size>::RawSwap(RawVector<size>& b)
{
	::Swap(items, b.items);
	::Swap(alloc, b.alloc);
	::Swap(vector, b.vector);
}

template <int size>
void RawVector<size>::RawReserve(int n)
{
	if(n > alloc)
		Realloc(n);
}

template <int size>
void RawVector<size>::RawInsert(int q, int count)
{
	Chk();
	ASSERT(q >= 0 && q <= items);
	if(!count) return;
	if(items + count > alloc) {
		Data *newvector = Alloc(alloc = alloc + ntl_max(alloc, count));
		if(vector) {
			memcpy(newvector, vector, q * sizeof(Data));
			memcpy(newvector + q + count, vector + q, (items - q) * sizeof(Data));
			RawFree();
		}
		vector = newvector;
	}
	else
		memmove(vector + q + count, vector + q, (items - q) * sizeof(Data));
	items += count;
}

template <int size>
void RawVector<size>::RawInsertPick(int i, pick_ RawVector<size>& v) {
	Chk();
	v.Chk();
	if(v.items) {
		RawInsert(i, v.items);
		memcpy(vector + i, v.vector, size * v.items);
	}
	const_cast< RawVector<size>& >(v).RawFree();
	SetPicked(v);
}

// ------------------

template <class T>
void Vector<T>::Free() {
	if(B::vector && B::items >= 0)
		DestroyArray((T *)B::vector, (T *)B::vector + B::items);
	B::RawFree();
}

template <class T>
void Vector<T>::Clear() {
	if(B::vector && B::items >= 0)
		SetCount(0);
	else {
		B::alloc = B::items = 0;
		B::vector = NULL;
	}
}

template <class T>
void Vector<T>::__DeepCopy(const Vector& src) {
	src.Chk();
	B::items = B::alloc = src.B::items;
	if(src.vector) {
		B::vector = B::Alloc(B::alloc);
		DeepCopyConstructArray((T *)B::vector, (T *)src.B::vector,
		                       (T *)src.B::vector + B::items);
	}
	else
		B::vector = NULL;
}

template <class T> inline
T&  Vector<T>::Add()
{
	if(B::items >= B::alloc) Expand();
	T *p = ::new(B::vector + B::items) T;
	B::items++;
	return *p;
}

template <class T> inline
void Vector<T>::AddN(int n)
{
	ASSERT(n >= 0);
	if(B::items + n <= B::alloc) {
		const TYPENAME Vector<T>::Data *w = B::vector + B::items;
		B::items += n;
		while(n--)
			::new((void *)w++) T;
	}
	else
		SetCountR(B::items + n);
}

template <class T>
void Vector<T>::Trim(int n)
{
	ASSERT(n >= 0 && n <= B::items);
	DestroyArray((T*)B::vector + n, (T*)B::vector + B::items);
	B::items = n;
}

template <class T>
void Vector<T>::SetCount(int n) {
	Chk();
	ASSERT(n >= 0);
	if(n == B::items) return;
	if(n < B::items)
		Trim(n);
	else {
		if(n > B::alloc) B::Realloc(n);
		ConstructArray((T*)B::vector + B::items, (T*)B::vector + n);
		B::items = n;
	}
}

template <class T>
void Vector<T>::SetCount(int n, const T& init) {
	Chk();
	ASSERT(n >= 0);
	if(n == B::items) return;
	if(n < B::items)
		DestroyArray((T*)B::vector + n, (T*)B::vector + B::items);
	else {
		if(n > B::alloc) B::Realloc(n);
		DeepCopyConstructFill((T*)B::vector + B::items, (T*)B::vector + n, init);
	}
	B::items = n;
}

template <class T>
void Vector<T>::SetCountR(int n) {
	Chk();
	if(n + B::items > B::alloc)
		B::Realloc(B::alloc + ntl_max(B::alloc, n));
	SetCount(n);
}

template <class T>
void Vector<T>::SetCountR(int n, const T& init) {
	Chk();
	if(n + B::items > B::alloc)
		B::Realloc(B::alloc + ntl_max(B::alloc, n));
	SetCount(n, init);
}

template <class T>
void Vector<T>::Remove(int q, int count) {
	Chk();
	ASSERT(q >= 0 && q <= B::items - count && count >= 0);
	if(count == 0) return;
	DestroyArray((T*)B::vector + q, (T*)B::vector + q + count);
//G++
	memmove((T*)B::vector + q, (T*)B::vector + q + count, (B::items - q - count) * sizeof(T));
//	memmove((T*)vector + q, (T*)vector + q + count, (items - q - count) * sizeof(Data));
	B::items -= count;
}

template <int size>
class Data_S_ {
	byte filler[size];
};

template <class T>
void Vector<T>::Remove(const int *sorted_list, int n)
{
	Chk();
	if(!n) return;
	int pos = *sorted_list;
	int npos = pos;
	for(;;) {
		ASSERT(pos < B::items);
		if(pos == *sorted_list) {
			((T*)B::vector + pos)->T::~T();
			pos++;
			sorted_list++;
			if(--n == 0) break;
			ASSERT(*sorted_list >= pos);
		}
		else
			*((Data_S_<sizeof(T)>*)B::vector + npos++)
				= *((Data_S_<sizeof(T)>*)B::vector + pos++);
	}
	while(pos < B::items)
		*((Data_S_<sizeof(T)>*)B::vector + npos++) = *((Data_S_<sizeof(T)>*)B::vector + pos++);
	B::items = npos;
}

template <class T>
void Vector<T>::Remove(const Vector<int>& v)
{
	Remove(v, v.GetCount());
}

template <class T>
void Vector<T>::InsertN(int q, int count) {
	ASSERT(count >= 0);
	B::RawInsert(q, count);
	ConstructArray((T*) B::vector + q, (T*)B::vector + q + count);
}

template <class T>
void Vector<T>::Insert(int q, const T& x, int count) {
	if(!count) return;
	B::RawInsert(q, count);
	DeepCopyConstructFill((T*)B::vector + q, (T*)B::vector + q + count, x);
}

template <class T>
void Vector<T>::Insert(int q, const Vector& x, int offset, int count) {
	if(!count) return;
	ASSERT(offset >= 0 && count >= 0 && offset + count <= x.GetCount());
	B::RawInsert(q, count);
	DeepCopyConstructArray((T*)B::vector + q, (T*)x.B::vector + offset,
	                       (T*)x.B::vector + offset + count);
}

template <class T>
void Vector<T>::Insert(int q, const Vector& x) {
	if(!x.GetCount()) return;
	Insert(q, x, 0, x.GetCount());
}

template <class T>
void Vector<T>::Set(int i, const T& x, int count) {
	Chk();
	ASSERT(i >= 0 && count >= 0);
	if(count == 0) return;
	DoIndex(i + count - 1);
	Fill((T*)B::vector + i, (T*)B::vector + i + count, x);
}

// ------------------

template <class T>
void Array<T>::Free() {
	if(IsPicked()) return;
	for(int i = 0; i < vector.GetCount(); i++)
		delete (T *) vector[i];
}

template <class T>
void Array<T>::__DeepCopy(const Array<T>& v) {
	int n = v.GetCount();
	vector.SetCount(n);
	for(int i = 0; i < n; i++)
		vector[i] = DeepCopyNew(v[i]);
}

template <class T>
void  Array<T>::Trim(int n)
{
	ASSERT(n >= 0 && n <= GetCount());
	Del(vector.Begin() + n, vector.End());
	vector.Trim(n);
}

template <class T>
void Array<T>::SetCount(int n) {
	ASSERT(n >= 0);
	int  lc = vector.GetCount();
	Del(vector.Begin() + n, vector.End());
	vector.SetCount(n);
	Init(vector.Begin() + lc, vector.Begin() + n);
}

template <class T>
void Array<T>::SetCount(int n, const T& init) {
	ASSERT(n >= 0);
	int  lc = vector.GetCount();
	Del(vector.Begin() + n, vector.End());
	vector.SetCount(n);
	Init(vector.Begin() + lc, vector.Begin() + n, init);
}

template <class T>
void Array<T>::SetCountR(int n) {
	ASSERT(n >= 0);
	int  lc = vector.GetCount();
	Del(vector.Begin() + n, vector.End());
	vector.SetCountR(n);
	Init(vector.Begin() + lc, vector.Begin() + n);
}

template <class T>
void Array<T>::SetCountR(int n, const T& init) {
	ASSERT(n >= 0);
	int  lc = vector.GetCount();
	Del(vector.Begin() + n, vector.End());
	vector.SetCountR(n);
	Init(vector.Begin() + lc, vector.Begin() + n, init);
}

template <class T>
int  Array<T>::GetIndex(const T& item) const {
	for(void * const *ptr = vector.Begin(); ptr < vector.End(); ptr++)
		if(*ptr == (void *)&item) return ptr - vector.Begin();
	return -1;
}

template <class T>
void Array<T>::Move(int i1, int i2) {
	void *q = vector[i1];
	vector.Remove(i1);
	vector.Insert(i2) = q;
}

template <class T>
void Array<T>::Remove(int i, int count) {
	ASSERT(i + count <= GetCount() && count >= 0 && i >= 0);
	Del(vector.Begin() + i, vector.Begin() + i + count);
	vector.Remove(i, count);
}

template <class T>
void Array<T>::Remove(const int *sorted_list, int n)
{
	const int *q = sorted_list;
	const int *e = sorted_list + n;
	while(q < e) {
		ASSERT(*q >= 0 && *q < GetCount());
		delete (T *)vector[*q++];
	}
	vector.Remove(sorted_list, n);
}

template <class T>
void Array<T>::Remove(const Vector<int>& sorted_list)
{
	Remove(sorted_list, sorted_list.GetCount());
}

template <class T>
void Array<T>::Set(int i, const T& x, int count) {
	ASSERT(i >= 0 && count >= 0);
	if(i + count >= GetCount())
		SetCountR(i + count);
	for(void **ptr = vector.Begin() + i; ptr < vector.Begin() + i + count; ptr++) {
		delete (T *) *ptr;
		*ptr = new T(x);
	}
}

template <class T>
void Array<T>::InsertN(int i, int count) {
	ASSERT(i >= 0 && count >= 0);
	vector.InsertN(i, count);
	Init(vector.Begin() + i, vector.Begin() + i + count);
}

template <class T>
void Array<T>::Insert(int i, const T& x, int count) {
	vector.InsertN(i, count);
	Init(vector.Begin() + i, vector.Begin() + i + count, x);
}

template <class T>
void Array<T>::Insert(int i, T *newt) {
	vector.InsertN(i, 1);
	vector[i] = newt;
}

template <class T>
void Array<T>::Insert(int i, const Array& x) {
	Insert(i, x, 0, x.GetCount());
}

template <class T>
void Array<T>::Insert(int i, const Array& x, int offset, int count) {
	vector.InsertN(i, count);
	for(int q = 0; q < count; q++)
		vector[q + i] = DeepCopyNew(x[q + offset]);
}

template <class T>
void Array<T>::InsertPick(int i, pick_ Array& x) {
	Array xx = x;
	int cx = xx.GetCount();
	vector.InsertN(i, cx);
	for(int q = 0; q < cx; q++)
		vector[q + i] = x.vector[q];
}

// ------------------

template <class T, int NBLK>
Segtor<T, NBLK>::Segtor(const Segtor& s, int) {
	items = s.items;
	block.SetCount((items + NBLK - 1) / NBLK);
	int q = items / NBLK;
	int r = items % NBLK;
	int i;
	for(i = 0; i < q; i++) {
		T *a = (T *) s.block[i].item;
		DeepCopyConstructArray((T *) block[i].item, a, a + NBLK);
	}
	if(r) {
		T *a = (T *) s.block[q].item;
		DeepCopyConstructArray((T *) block[i].item, a, a + r);
	}
}

template <class T, int NBLK>
void Segtor<T, NBLK>::Free() {
	int q = items / NBLK;
	int r = items % NBLK;
	int i;
	for(i = 0; i < q; i++) {
		T *a = (T *) block[i].item;
		DestroyArray(a, a + NBLK);
	}
	if(r) {
		T *a = (T *) block[i].item;
		DestroyArray(a, a + r);
	}
}

template <class T, int NBLK>
Segtor<T, NBLK>::~Segtor() {
	if(IsPicked()) return;
	Free();
}

template <class T, int NBLK>
void Segtor<T, NBLK>::DoRange(unsigned beg, unsigned end, void (*fn)(T*, const T*)) {
	ASSERT(beg <= end);
	int bblk = beg / NBLK, bidx = beg % NBLK;
	int eblk = end / NBLK, eidx = end % NBLK;
	if(eblk == block.GetCount()) {
		ASSERT(eidx == 0);
		eblk--;
		eidx = NBLK;
	}
	ASSERT(eblk < block.GetCount());
	T *tp = (T *)block[bblk].item;
	if(bblk != eblk) {
		(*fn)(tp + bidx, tp + NBLK);
		while(++bblk < eblk) {
			tp = (T *)block[bblk].item;
			(*fn)(tp, tp + NBLK);
		}
		tp = (T *)block[bblk].item;
		(*fn)(tp, tp + eidx);
	}
	else
		(*fn)(tp + bidx, tp + eidx);
}

template <class T, int NBLK>
void Segtor<T, NBLK>::Fill(unsigned beg, unsigned end, const T& x) {
	ASSERT(beg <= end);
	int bblk = beg / NBLK, bidx = beg % NBLK;
	int eblk = end / NBLK, eidx = end % NBLK;
	if(eblk == block.GetCount()) {
		ASSERT(eidx == 0);
		eblk--;
		eidx = NBLK;
	}
	ASSERT(eblk < block.GetCount());
	T *tp = (T *)block[bblk].item;
	if(bblk != eblk) {
		DeepCopyConstructFill(tp + bidx, tp + NBLK, x);
		while(++bblk < eblk) {
			tp = (T *)block[bblk].item;
			DeepCopyConstructFill(tp, tp + NBLK, x);
		}
		tp = (T *)block[bblk].item;
		DeepCopyConstructFill(tp, tp + eidx, x);
	}
	else
		DeepCopyConstructFill(tp + bidx, tp + eidx, x);
}

template <class T, int NBLK>
void Segtor<T, NBLK>::SetCount(int n) {
	Del(n);
	block.SetCount((n + NBLK - 1) / NBLK);
	Init(n);
}

template <class T, int NBLK>
void Segtor<T, NBLK>::SetCount(int n, const T& init) {
	Del(n);
	block.SetCount((n + NBLK - 1) / NBLK);
	Init(n, init);
}

template <class T, int NBLK>
void Segtor<T, NBLK>::Clear() {
	if(!IsPicked())
		Free();
	items = 0;
	block.Clear();
}

template <class T, int NBLK>
void Segtor<T, NBLK>::Set(int i, const T& x, int count) {
	ASSERT(i >= 0 && count >= 0);
	DoIndex(i + count - 1);
	Iterator q(*this, i);
	while(count--)
		*q++ = x;
}

template <class T, int NBLK>
int Segtor<T, NBLK>::GetIndex(const T& item) const {
	for(int i = 0; i < block.GetCount(); i++) {
		T *q = (T *) block[i].item;
		if(&item >= q && &item < q + NBLK)
			return &item - q + NBLK * i;
	}
	return -1;
}

// ------------------

template <class T>
void BiVector<T>::ReAlloc(int newalloc) {
	ASSERT(items <= newalloc && items >= 0);
	T *newvector = newalloc ? (T *) NTL_RAW_ALLOC(newalloc * sizeof(T)) : NULL;
	if(items) {
		int end = start + items;
		if(end <= alloc)
			memcpy(newvector, vector + start, (end - start) * sizeof(T));
		else {
			memcpy(newvector, vector + start, (alloc - start) * sizeof(T));
			memcpy(newvector + alloc - start, vector, (end - alloc) * sizeof(T));
		}
		NTL_RAW_FREE(vector, items, sizeof(T));
	}
	vector = newvector;
	alloc = newalloc;
	start = 0;
}

template <class T>
void BiVector<T>::DeepCopy0(const BiVector& src) {
	alloc = items = src.items;
	vector = alloc ? (T *) NTL_RAW_ALLOC((alloc * sizeof(T))) : NULL;
	if(items) {
		int end = src.start + src.items;
		if(end <= src.alloc)
			DeepCopyConstructArray(vector, src.vector + src.start, src.vector + end);
		else {
			DeepCopyConstructArray(vector, src.vector + src.start, src.vector + src.alloc);
			DeepCopyConstructArray(vector + src.alloc - src.start, src.vector,
				                   src.vector + end - src.alloc);
		}
	}
	start = 0;
}

template <class T>
void BiVector<T>::Clear() {
	Free();
	start = items = alloc = 0;

	vector = NULL;
}

template <class T>
void BiVector<T>::Add0() {
	ASSERT(items >= 0);
	if(items >= alloc)
		ReAlloc(ntl_max(2 * items, 4));
	items++;
}

template <class T>
void BiVector<T>::Shrink() {
	ASSERT(items >= 0);
	if(items < alloc)
		ReAlloc(items);
}

template <class T>
void BiVector<T>::Reserve(int n) {
	ASSERT(items >= 0);
	n += items;
	if(n > alloc)
		ReAlloc(n);
}

template <class T>
void BiVector<T>::Free() {
	if(vector && items >= 0) {
		int end = start + items;
		if(end <= alloc)
			DestroyArray(vector + start, vector + end);
		else {
			DestroyArray(vector + start, vector + alloc);
			DestroyArray(vector, vector + end - alloc);
		}
		NTL_RAW_FREE(vector, items, sizeof(T));
	}
}

#ifdef UPP
template <class T>
void BiVector<T>::Serialize(Stream& s) {
	int n = items;
	s / n;
	if(s.IsLoading()) {
		Clear();
		while(n--)
			s % AddTail();
	}
	else
		for(int i = 0; i < items; i++)
			s % operator[](i);
}
#endif

// ------------------

template <class T>
void BiArray<T>::Free() {
	if(!bv.IsPicked())
		for(int i = 0; i < GetCount(); i++)
			delete (T *) bv[i];
}

template <class T>
void BiArray<T>::DeepCopy0(const BiArray<T>& v) {
	int n = v.GetCount();
	bv.Clear();
	bv.Reserve(v.GetCount());
	for(int i = 0; i < n; i++)
		bv.AddTail() = DeepCopyNew(v[i]);
}

#ifdef UPP
template <class T>
void BiArray<T>::Serialize(Stream& s) {
	int n = bv.GetCount();
	s / n;
	if(s.IsLoading()) {
		Clear();
		while(n--)
			s % AddTail();
	}
	else
		for(int i = 0; i < bv.GetCount(); i++)
			s % operator[](i);
}
#endif
