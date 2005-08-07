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
class RawVector {
protected:
	struct Data {
		byte filler[size];
	};

	Data    *vector;
	int      items;
	int      alloc;

	void     RawFree();
	Data    *Alloc(int n)            { return (Data *) NTL_RAW_ALLOC(n * sizeof(Data)); }
	void     Realloc(int newalloc);

	void     Chk() const             { ASSERT(items >= 0); }
	void     Pick(pick_ RawVector& v);
	void     SetPicked(pick_ RawVector& v) {
		RawVector& pick = (RawVector&)(v);
		pick.items = -1;
		pick.vector = NULL;
	}

	void     Expand();

	void     RawInsert(int q, int count);
	void     RawInsertPick(int i, pick_ RawVector<size>& v);

	void    *RawAdd()                { Chk(); if(items >= alloc) Expand();
		                               return vector + items++; }

	int      RawGetIndex(const void *item) const;

	RawVector()                      { vector = NULL; items = alloc = 0; }

	void     RawReserve(int xtra);
	void     RawSwap(RawVector& b);
};

template <class T>
class Vector : public RawVector<sizeof(T)>
               , Moveable< Vector<T> >
               , DeepCopyOption< Vector<T> > {
	typedef RawVector<sizeof(T)> B;

	void     Free();
	void     __DeepCopy(const Vector& src);
	T&       Get(int i) const        { ASSERT(i >= 0 && i < B::items); return (T&)B::vector[i]; }
	void     Chk() const             { B::Chk(); }
	void     Expand()                { B::Expand(); }

public:
	T&       Add();
	void     Add(const T& x)         { DeepCopyConstruct(B::RawAdd(), x); }
	void     AddPick(pick_ T& x)     { ::new(B::RawAdd()) T(x); }
	void     AddN(int n);
	const T& operator[](int i) const { return Get(i); }
	T&       operator[](int i)       { return Get(i); }
	int      GetCount() const        { Chk(); return B::items; }
	bool     IsEmpty() const         { Chk(); return B::items == 0; }
	void     Trim(int n);
	void     SetCount(int n);
	void     SetCount(int n, const T& init);
	void     SetCountR(int n);
	void     SetCountR(int n, const T& init);
	void     Clear();

	T&       At(int i)                  { if(i >= B::items) SetCountR(i + 1); return (*this)[i]; }
	T&       At(int i, const T& x)      { if(i >= B::items) SetCountR(i + 1, x); return (*this)[i]; }

	void     Shrink()                   { B::Realloc(B::items); }
	void     Reserve(int n)             { B::RawReserve(n); }
	int      GetAlloc() const           { return B::alloc; }

	void     Set(int i, const T& x, int count = 1);
	void     Remove(int i, int count = 1);
	void     Remove(const int *sorted_list, int n);
	void     Remove(const Vector<int>& sorted_list);
	void     InsertN(int i, int count = 1);
	T&       Insert(int i)           { InsertN(i); return Get(i); }
	void     Insert(int i, const T& x, int count = 1);
	void     Insert(int i, const Vector& x);
	void     Insert(int i, const Vector& x, int offset, int count);
	void     InsertPick(int i, pick_ Vector& x)    { B::RawInsertPick(i, x); }
	void     Append(const Vector& x)               { Insert(GetCount(), x); }
	void     Append(const Vector& x, int o, int c) { Insert(GetCount(), x, o, c); }
	void     AppendPick(pick_ Vector& x)           { B::RawInsertPick(GetCount(), x); }
	int      GetIndex(const T& item) const         { return B::RawGetIndex(&item); }

	void     Drop(int n = 1)         { ASSERT(n <= GetCount()); Trim(B::items - n); }
	T&       Top()                   { ASSERT(GetCount()); return Get(B::items - 1); }
	const T& Top() const             { ASSERT(GetCount()); return Get(B::items - 1); }
	T        Pop()                   { T h = Top(); Drop(); return h; }

	operator T*()                    { return (T*)B::vector; }
	operator const T*() const        { return (T*)B::vector; }

	Vector&  operator<<(const T& x)  { Add(x); return *this; }
	Vector&  operator|(pick_ T& x)   { AddPick(x); return *this; }

#ifdef UPP
	void     Serialize(Stream& s)    { StreamContainer(s, *this); }
#endif

	Vector()                         {}
	~Vector()                        { AssertMoveable<T>(); Free(); }

// Pick assignment & copy. Picked source can only Clear(), ~Vector(), operator=, operator <<=
	Vector(pick_ Vector& v) : B()    { B::Pick(v); }
	void operator=(pick_ Vector& v)  { Free(); B::Pick(v); }
	bool IsPicked() const            { return B::items < 0; }

// Deep copy
	Vector(const Vector& v, int)     { __DeepCopy(v); }

// Standard container interface
	typedef T        ValueType;
	typedef T       *Iterator;
	typedef const T *ConstIterator;

	ConstIterator    Begin() const          { return (T*)B::vector; }
	ConstIterator    End() const            { return (T*)B::vector + B::items; }
	ConstIterator    GetIter(int i) const   { ASSERT(i >= 0 && i <= B::items); return Begin() + i; }
	Iterator         Begin()                { return (T*)B::vector; }
	Iterator         End()                  { return (T*)B::vector + B::items; }
	Iterator         GetIter(int i)         { ASSERT(i >= 0 && i <= B::items); return Begin() + i; }

// Optimalizations
	friend void Swap(Vector& a, Vector& b)                     { a.B::RawSwap(b); }
	friend void Append(Vector& dst, const Vector& src)         { dst.Append(src); }

//obsolete names
	T&       DoIndex(int i)             { return At(i); }
	T&       DoIndex(int i, const T& x) { return At(i, x); }

	STL_VECTOR_COMPATIBILITY(Vector<T>)
};

template <class T>
class Array : Moveable< Array<T> >, DeepCopyOption< Array<T> > {
protected:
	Vector<void *> vector;

	void     Free();
	void     __DeepCopy(const Array& v);
	T&       Get(int i) const                           { return *(T *)vector[i]; }
	void     Del(void **ptr, void **lim)                { while(ptr < lim) delete (T *) *ptr++; }
	void     Init(void **ptr, void **lim)               { while(ptr < lim) *ptr++ = new T; }
	void     Init(void **ptr, void **lim, const T& x)   { while(ptr < lim) *ptr++ = DeepCopyNew(x); }

public:
	T&       Add()                      { T *q = new T; vector.Add(q); return *q; }
	void     Add(const T& x)            { vector.Add(DeepCopyNew(x)); }
	void     AddPick(pick_ T& x)        { vector.Add(new T(x)); }
	T&       Add(T *newt)               { vector.Add(newt); return *newt; }
	const T& operator[](int i) const    { return Get(i); }
	T&       operator[](int i)          { return Get(i); }
	int      GetCount() const           { return vector.GetCount(); }
	bool     IsEmpty() const            { return vector.IsEmpty(); }
	void     Trim(int n);
	void     SetCount(int n);
	void     SetCount(int n, const T& init);
	void     SetCountR(int n);
	void     SetCountR(int n, const T& init);
	void     Clear()                    { Free(); vector.Clear(); }

	T&       At(int i)                  { if(i >= GetCount()) SetCountR(i + 1); return Get(i); }
	T&       At(int i, const T& x)      { if(i >= GetCount()) SetCountR(i + 1, x); return Get(i); }

	void     Shrink()                   { vector.Shrink(); }
	void     Reserve(int xtra)          { vector.Reserve(xtra); }
	int      GetAlloc() const           { return vector.GetAlloc(); }

	void     Set(int i, const T& x, int count = 1);
	void     Remove(int i, int count = 1);
	void     Remove(const int *sorted_list, int n);
	void     Remove(const Vector<int>& sorted_list);
	void     InsertN(int i, int count = 1);
	T&       Insert(int i)              { InsertN(i); return Get(i); }
	void     Insert(int i, const T& x, int count = 1);
	void     Insert(int i, const Array& x);
	void     Insert(int i, const Array& x, int offset, int count);
	void     Append(const Array& x)               { Insert(GetCount(), x); }
	void     Append(const Array& x, int o, int c) { Insert(GetCount(), x, o, c); }
	void     InsertPick(int i, pick_ Array& x);
	void     AppendPick(pick_ Array& x)           { Insert(GetCount(), x); }
	int      GetIndex(const T& item) const;
	void     Swap(int i1, int i2)       { ::Swap(vector[i1], vector[i2]); }
	void     Move(int i1, int i2);

	T       *Detach(int i)              { T *t = &Get(i); vector.Remove(i); return t; }
	T&       Set(int i, T *newt)        { delete (T *)vector[i]; vector[i] = newt; return *newt; }
	void     Insert(int i, T *newt);

	void     Drop(int n = 1)            { Trim(GetCount() - n); }
	T&       Top()                      { return Get(GetCount() - 1); }
	const T& Top() const                { return Get(GetCount() - 1); }
//	T        Pop()                      { T h = Top(); Drop(); return h; } // msvc bug
	T       *PopDetach()                { return (T *) vector.Pop(); }

	void     Swap(Array& b)             { Swap(vector, b.vector); }

	Array& operator<<(const T& x)       { Add(x); return *this; }
	Array& operator<<(T *newt)          { Add(newt); return *this; }
	Array& operator|(pick_ T& x)        { AddPick(x); return *this; }

	bool     IsPicked() const           { return vector.IsPicked(); }

#ifdef UPP
	void     Serialize(Stream& s)       { StreamContainer(s, *this); }
#endif

	Array()                             {}
	~Array()                            { Free(); }

// Pick assignment & copy. Picked source can only Clear(), ~Vector(), operator=, operator<<=
	Array(pick_ Array& v) : vector(v.vector)  {}
	void operator=(pick_ Array& v)            { Free(); vector = v.vector; }

// Deep copy
	Array(const Array& v, int)          { __DeepCopy(v); }

	class Iterator;

	class ConstIterator {
	protected:
		T **ptr;
		ConstIterator(T **p)                    { ptr = p; }

		friend class Array<T>;
		struct NP { int dummy; };

	public:
		const T& operator*() const              { return **ptr; }
		const T *operator->() const             { return *ptr; }
		const T& operator[](int i) const        { return *ptr[i]; }

		ConstIterator& operator++()             { ptr++; return *this; }
		ConstIterator& operator--()             { ptr--; return *this; }
		ConstIterator  operator++(int)          { ConstIterator t = *this; ++*this; return t; }
		ConstIterator  operator--(int)          { ConstIterator t = *this; --*this; return t; }

		ConstIterator& operator+=(int i)        { ptr += i; return *this; }
		ConstIterator& operator-=(int i)        { ptr -= i; return *this; }

		ConstIterator operator+(int i) const    { return ptr + i; }
		ConstIterator operator-(int i) const    { return ptr - i; }

		int  operator-(ConstIterator x) const   { return ptr - x.ptr; }

		bool operator==(ConstIterator x) const  { return ptr == x.ptr; }
		bool operator!=(ConstIterator x) const  { return ptr != x.ptr; }
		bool operator<(ConstIterator x) const   { return ptr < x.ptr; }
		bool operator>(ConstIterator x) const   { return ptr > x.ptr; }
		bool operator<=(ConstIterator x) const  { return ptr <= x.ptr; }
		bool operator>=(ConstIterator x) const  { return ptr >= x.ptr; }

		operator bool() const                   { return ptr; }

		ConstIterator()                         {}
		ConstIterator(NP *null)                 { ASSERT(null == NULL); ptr = NULL; }
	};

	class Iterator : public ConstIterator {
		friend class Array<T>;
		Iterator(T **p) : ConstIterator(p)      {}
		typedef ConstIterator B;
		struct NP { int dummy; };

	public:
		T& operator*()                          { return **B::ptr; }
		T *operator->()                         { return *B::ptr; }
		T& operator[](int i)                    { return *B::ptr[i]; }

		const T& operator*() const              { return **B::ptr; }
		const T *operator->() const             { return *B::ptr; }
		const T& operator[](int i) const        { return *B::ptr[i]; }

		Iterator& operator++()                  { B::ptr++; return *this; }
		Iterator& operator--()                  { B::ptr--; return *this; }
		Iterator  operator++(int)               { Iterator t = *this; ++*this; return t; }
		Iterator  operator--(int)               { Iterator t = *this; --*this; return t; }

		Iterator& operator+=(int i)             { B::ptr += i; return *this; }
		Iterator& operator-=(int i)             { B::ptr -= i; return *this; }

		Iterator operator+(int i) const         { return B::ptr + i; }
		Iterator operator-(int i) const         { return B::ptr - i; }

		int      operator-(Iterator x) const    { return B::operator-(x); }

		Iterator()                               {}
		Iterator(NP *null) : ConstIterator(null) {}

	//G++
	//	friend void IterSwap(Iterator a, Iterator b) { ::Swap(*a.ptr, *b.ptr); }
	};

// Standard container interface
	typedef T        ValueType;
	Iterator         Begin()                    { return (T **)vector.Begin(); }
	Iterator         End()                      { return (T **)vector.End(); }
	Iterator         GetIter(int pos)           { return (T **)vector.GetIter(pos); }
	ConstIterator    Begin() const              { return (T **)vector.Begin(); }
	ConstIterator    End() const                { return (T **)vector.End(); }
	ConstIterator    GetIter(int pos) const     { return (T **)vector.GetIter(pos); }

// Optimalization
	friend void Swap(Array& a, Array& b)                   { ::Swap(a.vector, b.vector); }
	//GCC forced move from Iterator, ugly workaround
private:
	static void IterSwap0(Iterator a, Iterator b)          { ::Swap(*a.ptr, *b.ptr); }
public:
	friend void IterSwap(Iterator a, Iterator b)           { Array<T>::IterSwap0(a, b); }

//obsolete names
	T&       DoIndex(int i)             { return At(i); }
	T&       DoIndex(int i, const T& x) { return At(i, x); }

	STL_VECTOR_COMPATIBILITY(Array<T>)
};

template<class T, int NBLK = 16>
class Segtor : Moveable< Segtor<T, NBLK> >, DeepCopyOption< Segtor<T, NBLK> > {
protected:
	struct Block {
		byte item[NBLK][sizeof(T)];
	};

	Array<Block> block;
	int          items;

	void  DoRange(unsigned beg, unsigned end, void (*fn)(T*, const T*));
	void  Fill(unsigned beg, unsigned end, const T& x);
	T&    Get(int i) const {
		ASSERT(i >= 0 && i < items);
		return *(T*) block[unsigned(i) / NBLK].item[unsigned(i) % NBLK];
	}
	void *Add0()				     {
		int blk = unsigned(items) / NBLK, ndx = unsigned(items) % NBLK;
		if(ndx == 0) block.Add(); items++;
		return block[blk].item[ndx];
	}
	void  Del(int n)              { if(n < items) DoRange(n, items, DestroyArray); }
	void  Init(int n)             { if(n > items) DoRange(items, n, ConstructArray); items = n; }
	void  Init(int n, const T& x) { if(n > items) Fill(items, n, x); items = n; }
	void  Free();

public:
	T&       Add()                          { return *::new(Add0()) T; }
	void     Add(const T& x)                { DeepCopyConstruct(Add0(), x); }
	void     AddPick(pick_ T& x)            { ::new(Add0()) T; }
	T&       operator[](int i)              { return Get(i); }
	const T& operator[](int i) const        { return Get(i); }
	int      GetCount() const               { return items; }
	bool     IsEmpty() const                { return items == 0; }
	void     SetCount(int n);
	void     SetCount(int n, const T& init);
	void     Clear();
	T&       At(int i)                      { if(i >= items) SetCount(i + 1); return Get(i); }
	T&       At(int i, const T& x)          { if(i >= items) SetCount(i + 1, x); return Get(i); }
	void     Shrink()                       { block.Shrink(); }
	void     Reserve(int xtra)              { block.Reserve((xtra + NBLK - 1) / NBLK); }
	int      GetAlloc() const               { return block.GetAlloc() * NBLK; }

	void     Set(int i, const T& x, int count = 1);
	int      GetIndex(const T& item) const;

	void     Drop(int n = 1)                { ASSERT(n <= GetCount()); SetCount(GetCount() - n); }
	T&       Top()                          { ASSERT(GetCount()); return Get(GetCount() - 1); }
	const T& Top() const                    { ASSERT(GetCount()); return Get(GetCount() - 1); }
	T        Pop()                          { T h = Top(); Drop(); return h; }

	void     Swap(Segtor& b)                { block.Swap(b.block); Swap(items, b.items); }

	Segtor& operator<<(const T& x)          { Add(x); return *this; }
	Segtor& operator|(pick_ T& x)           { AddPick(x); return *this; }

	bool     IsPicked() const               { return block.IsPicked(); }

#ifdef UPP
	void     Serialize(Stream& s)           { StreamContainer(s, *this); }
#endif

	Segtor()                                { items = 0; }
	Segtor(pick_ Segtor& s) : block(s.block), items(s.items) {}
	Segtor(const Segtor& s, int);
	~Segtor();

// Standard iterators
	typedef ConstIIterator<Segtor> ConstIterator;
	typedef IIterator<Segtor>      Iterator;

// Standard container interface
	typedef T        ValueType;
	ConstIterator    Begin() const              { return ConstIterator(*this, 0); }
	ConstIterator    End() const                { return ConstIterator(*this, items); }
	ConstIterator    GetIter(int pos) const     { return ConstIterator(*this, pos); }
	Iterator         Begin()                    { return Iterator(*this, 0); }
	Iterator         End()                      { return Iterator(*this, items); }
	Iterator         GetIter(int pos)           { return Iterator(*this, pos); }

// Optimizations
	friend void Swap(Segtor& a, Segtor& b)      { a.Swap(b); }

//obsolete names
	T&       DoIndex(int i)             { return At(i); }
	T&       DoIndex(int i, const T& x) { return At(i, x); }

// traits
	STL_VECTOR_COMPATIBILITY(Segtor<T _cm_ NBLK>)
};
