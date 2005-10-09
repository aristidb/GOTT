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

#ifdef UPP
template <class T>
T& Single() {
	static T *p;
	if(!p) {
		CriticalSection::LockMain __;
		if(!p) {
			static T o;
			p = &o;
		}
	}
	return *p;
}
#endif

template <class T>
class One : Moveable< One<T> >, DeepCopyOption< One<T> > {
	mutable T  *ptr;

	void        Free()                     { if(ptr && ptr != (T*)1) delete ptr; }
	void        Chk() const                { ASSERT(ptr != (T*)1); }
	void        ChkP() const               { Chk(); ASSERT(ptr); }
	void        Pick(pick_ One<T>& data)   { T *p = data.ptr; data.ptr = (T*)1; ptr = p; }

public:
	void        Attach(T *data)            { Free(); ptr = data; }
	T          *Detach() pick_             { ChkP(); T *t = ptr; ptr = NULL; return t; }
	T          *operator-() pick_          { return Detach(); }
	void        Clear()                    { Free(); ptr = NULL; }

	void        operator=(T *data)         { Attach(data); }
	void        operator=(pick_ One<T>& d) { Free(); Pick(d); }

	const T    *operator->() const         { ChkP(); return ptr; }
	T          *operator->()               { ChkP(); return ptr; }
	const T    *operator~() const          { Chk(); return ptr; }
	T          *operator~()                { Chk(); return ptr; }
	const T&    operator*() const          { ChkP(); return *ptr; }
	T&          operator*()                { ChkP(); return *ptr; }

	bool        IsPicked() const           { return ptr == (T*)1; }
	bool        IsEmpty() const            { Chk(); return !ptr; }

	operator bool() const                  { return ptr; }

	One()                                  { ptr = NULL; }
	One(T *newt)                           { ptr = newt; }
	One(pick_ One<T>& p)                   { Pick(p); }
	One(const One<T>& p, int)              { ptr = p.IsEmpty() ? NULL : DeepCopyNew(*p); }
	~One()                                 { Free(); }
};

//# System dependent
template <class T>
class Mitor : Moveable< Mitor<T> > {
	union {
		mutable unsigned   count;
		mutable Vector<T> *vector;
	};
	byte elem0[sizeof(T)];
	
	T&        Get(int i) const;
	void      Pick(pick_ Mitor& m);
	void      Copy(const Mitor& m);
	void      Chk() const               { ASSERT(count != 2); }

public:
	T&        operator[](int i)         { return Get(i); }
	const T&  operator[](int i) const   { return Get(i); }
	int       GetCount() const;
	T&        Add();
	void      Add(const T& x);
	void      Clear();
	void      Shrink();
	
	Mitor(pick_ Mitor& m)               { Pick(m); }
	void operator=(pick_ Mitor& m)      { Clear(); Pick(m); }

	Mitor(Mitor& m, int)                { Copy(m); }
	void operator<<=(const Mitor& m)    { Clear(); Copy(m); }

	Mitor()                             { count = 0; }
	~Mitor()                            { Clear(); }
};

template <class T>
T& Mitor<T>::Get(int i) const
{
	ASSERT(i >= 0 && i < GetCount());
	return i == 0 ? *(T*)elem0 : (*const_cast<Vector<T>*>(vector))[i - 1];
}

template <class T>
void Mitor<T>::Pick(pick_ Mitor& m)
{
	m.Chk();
	vector = m.vector;
	memcpy(&elem0, &m.elem0, sizeof(T));
	m.count = 2;
}

template <class T>
void Mitor<T>::Copy(const Mitor& m)
{
	m.Chk();
	if(m.count > 0)
		DeepCopyConstruct((T*)elem0, m.elem0);
	if(m.count > 1)
		vector = new Vector<T>(m.vector, 1);
}

template <class T>
int Mitor<T>::GetCount() const
{
	Chk();
	return count > 1 ? vector->GetCount() + 1 : count;
}

template <class T>
T& Mitor<T>::Add()
{
	Chk();
	if(count == 0) {
		count = 1;
		return *new(elem0) T;
	}
	if(count == 1)
		vector = new Vector<T>;
	return vector->Add();
}

template <class T>
void Mitor<T>::Add(const T& x)
{
	Chk();
	if(count == 0) {
		count = 1;
		new((T*) elem0) T(x);
	}
	else {
		if(count == 1)
			vector = new Vector<T>;
		vector->Add(x);
	}
}

template <class T>
void Mitor<T>::Clear()
{
	if(count > 2)
		delete vector;
	if(count && count != 2)
		((T*)elem0)->T::~T();
	count = 0;
}

template <class T>
void Mitor<T>::Shrink()
{
	if(count > 2)
		vector->Shrink();
}

//#
template <class T, int N = 1>
class Link {
protected:
	T *prev[N];
	T *next[N];

	void LPN(int i)                      { prev[i]->next[i] = next[i]->prev[i] = (T *)this; }

public:
	T       *GetPtr()                    { return (T *) this; }
	const T *GetPtr() const              { return (const T *) this; }
	T       *GetNext(int i = 0)          { return next[i]; }
	T       *GetPrev(int i = 0)          { return prev[i]; }
	const T *GetNext(int i = 0) const    { return next[i]; }
	const T *GetPrev(int i = 0) const    { return prev[i]; }

	void LinkSelf(int i = 0)             { next[i] = prev[i] = (T *)this; }
	void LinkSelfAll()                   { for(int i = 0; i < N; i++) LinkSelf(i); }
	void Unlink(int i = 0)               { next[i]->prev[i] = prev[i]; prev[i]->next[i] = next[i];
	                                       LinkSelf(i); }
	void UnlinkAll()                     { for(int i = 0; i < N; i++) Unlink(i); }
	void LinkBefore(Link *n, int i = 0)  { next[i] = (T *)n; prev[i] = next[i]->prev[i]; LPN(i); }
	void LinkAfter(Link *p, int i = 0)   { prev[i] = (T *)p; next[i] = prev[i]->next[i]; LPN(i); }

	T   *InsertNext(int i = 0)           { T *x = new T; x->LinkAfter(this, i); return x; }
	T   *InsertPrev(int i = 0)           { T *x = new T; x->LinkBefore(this, i); return x; }

	void DeleteList(int i = 0)           { while(next[i] != GetPtr()) delete next[i]; }

	bool InList(int i = 0) const         { return next[i] != GetPtr(); }
	bool IsEmpty(int i = 0) const        { return !InList(i); }

	Link()                               { LinkSelfAll(); }
	~Link()                              { UnlinkAll(); }

private:
	Link(const Link&);
	void operator=(const Link&);

public:
#ifdef _DEBUG
	void Dump() {
		for(T *t = GetNext(); t != this; t = t->GetNext())
			LOG(t);
		LOG("-------------------------------------");
	}
#endif
};

template <class T, int N = 1>
class LinkOwner : public Link<T, N> {
public:
	~LinkOwner()                         { Link<T, N>::DeleteList(); }
};
