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
class BiVector : Moveable< BiVector<T> >, DeepCopyOption< BiVector<T> > {
protected:
	T          *vector;
	int         start;
	mutable int items;
	int         alloc;

	int      EI() const              { return (start + items - 1) % alloc; }
	void     ReAlloc(int newalloc);
	void     Add0();
	void     DeepCopy0(const BiVector& src);
	T       *AddHead0()              { Add0(); return &vector[start = (start + alloc - 1) % alloc]; }
	T       *AddTail0()              { Add0(); return &vector[EI()]; }
	void     Free();
	void     Pick(pick_ BiVector& x) { vector = x.vector; start = x.start; items = x.items;
	                                   alloc = x.alloc; x.items = -1; }
	void     Copy(T *dst, int start, int count) const;

public:
	int      GetCount() const        { return items; }
	bool     IsEmpty() const         { return items == 0; }
	void     Clear();

	T&       AddHead()               { return *new(AddHead0()) T; }
	T&       AddTail()               { return *new(AddTail0()) T; }
	void     AddHead(const T& x)     { new(AddHead0()) T(x); }
	void     AddTail(const T& x)     { new(AddTail0()) T(x); }
	void     AddHeadPick(pick_ T& x) { new(AddHead0()) T(x); }
	void     AddTailPick(pick_ T& x) { new(AddTail0()) T(x); }
	T&       Head()                  { ASSERT(items > 0); return vector[start]; }
	T&       Tail()                  { ASSERT(items > 0); return vector[EI()]; }
	const T& Head() const            { ASSERT(items > 0); return vector[start]; }
	const T& Tail() const            { ASSERT(items > 0); return vector[EI()]; }
	void     DropHead()              { (&Head())->T::~T(); items--; start = (start + 1) % alloc; }
	void     DropTail()              { (&Tail())->T::~T(); items--; }

	T&       operator[](int i)       { ASSERT(i >= 0 && i < items);
	                                   return vector[(start + i) % alloc]; }
	const T& operator[](int i) const { ASSERT(i >= 0 && i < items);
	                                   return vector[(start + i) % alloc]; }
	void     Shrink();
	void     Reserve(int n);
	int      GetAlloc() const        { return alloc; }

#ifdef UPP
	void     Serialize(Stream& s);
#endif

	bool     IsPicked()                         { return items < 0; }

	BiVector(const BiVector& src, int)          { DeepCopy0(src); }
	BiVector(pick_ BiVector& src)               { Pick(src); }
	void operator=(pick_ BiVector& src)         { Free(); Pick(src); }
	BiVector()                                  { start = items = alloc = 0; vector = NULL; }
	~BiVector()                                 { AssertMoveable<T>(); Free(); }

	typedef ConstIIterator<BiVector> ConstIterator;
	typedef IIterator<BiVector>      Iterator;

	typedef T        ValueType;

	ConstIterator    Begin() const              { return ConstIterator(*this, 0); }
	ConstIterator    End() const                { return ConstIterator(*this, GetCount()); }
	ConstIterator    GetIter(int pos) const     { return ConstIterator(*this, pos); }
	Iterator         Begin()                    { return Iterator(*this, 0); }
	Iterator         End()                      { return Iterator(*this, GetCount()); }
	Iterator         GetIter(int pos)           { return Iterator(*this, pos); }

	friend void Swap(BiVector& a, BiVector& b)  { ::Swap(a.vector, b.vector);
	                                              ::Swap(a.start, b.start);
	                                              ::Swap(a.items, b.items);
	                                              ::Swap(a.alloc, b.alloc); }

	STL_BI_COMPATIBILITY(BiVector<T>)
};

template <class T>
class BiArray : Moveable< BiArray<T> >, DeepCopyOption< BiArray<T> > {
protected:
	BiVector<void *> bv;

	void     Free();
	void     DeepCopy0(const BiArray<T>& v);

public:
	int      GetCount() const              { return bv.GetCount(); }
	bool     IsEmpty() const               { return GetCount() == 0; }
	void     Clear()                       { Free(); bv.Clear(); }

	T&       AddHead()                     { T *q = new T; bv.AddHead(q); return *q; }
	T&       AddTail()                     { T *q = new T; bv.AddTail(q); return *q; }
	void     AddHead(const T& x)           { bv.AddHead(new T(x)); }
	void     AddTail(const T& x)           { bv.AddTail(new T(x)); }
	void     AddHeadPick(pick_ T& x)       { bv.AddHead(new T(x)); }
	void     AddTailPick(pick_ T& x)       { bv.AddTail(new T(x)); }
	void     AddHead(T *newt)              { bv.AddHead(newt); }
	void     AddTail(T *newt)              { bv.AddTail(newt); }
	T&       Head()                        { return *(T *) bv.Head(); }
	T&       Tail()                        { return *(T *) bv.Tail(); }
	const T& Head() const                  { return *(const T *) bv.Head(); }
	const T& Tail() const                  { return *(const T *) bv.Tail(); }
	void     DropHead()                    { delete (T*) bv.Head(); bv.DropHead(); }
	void     DropTail()                    { delete (T*) bv.Tail(); bv.DropTail(); }
	T       *DetachHead()                  { T *q = (T*) bv.Head(); bv.DropHead(); return q; }
	T       *DetachTail()                  { T *q = (T*) bv.Tail(); bv.DropTail(); return q; }

	T&       operator[](int i)             { return *(T *) bv[i]; }
	const T& operator[](int i) const       { return *(const T *) bv[i]; }

	void     Shrink()                      { bv.Shrink(); }
	void     Reserve(int n)                { bv.Reserve(n); }
	int      GetAlloc() const              { return bv.GetAlloc(); }

#ifdef UPP
	void     Serialize(Stream& s);
#endif

	bool     IsPicked() const                { return bv.IsPicked(); }

	BiArray(const BiArray& v, int)           { DeepCopy0(v); }

	BiArray(pick_ BiArray& src) : bv(src.bv) {}
	void operator=(pick_ BiArray& src)       { Free(); bv = src.bv; }
	BiArray()                                {}
	~BiArray()                               { Free(); }

	typedef ConstIIterator<BiArray> ConstIterator;
	typedef IIterator<BiArray>      Iterator;

	typedef T        ValueType;

	ConstIterator    Begin() const              { return ConstIterator(*this, 0); }
	ConstIterator    End() const                { return ConstIterator(*this, GetCount()); }
	ConstIterator    GetIter(int pos) const     { return ConstIterator(*this, pos); }
	Iterator         Begin()                    { return Iterator(*this, 0); }
	Iterator         End()                      { return Iterator(*this, GetCount()); }
	Iterator         GetIter(int pos)           { return Iterator(*this, pos); }

	friend void Swap(BiArray& a, BiArray& b)    { ::Swap(a.bv, b.bv); }

	STL_BI_COMPATIBILITY(BiArray<T>)
};
