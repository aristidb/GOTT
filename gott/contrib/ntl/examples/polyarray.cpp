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

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <ntls.h>

struct BaseClass : PolyDeepCopyNew<BaseClass> {
	virtual char      *Name() const { return "This is the BaseClass"; }
	virtual BaseClass *Copy() const { return new BaseClass(); }
	virtual ~BaseClass() {}
	
	BaseClass() {}

private:
	BaseClass(const BaseClass&);
	void operator=(const BaseClass&);
};

// Copy method and PolyDeepCopyNew are needed
// only if deep copies of Array are required

struct Derived1 : BaseClass {
	virtual char      *Name() const { return "This is the Derived1"; }
	virtual BaseClass *Copy() const { return new Derived1(); }
};

struct Derived2 : BaseClass {
	virtual char      *Name() const { return "This is the Derived2"; }
	virtual BaseClass *Copy() const { return new Derived2(); }
};

bool operator<(const BaseClass& a, const BaseClass& b)
{
	return strcmp(a.Name(), b.Name()) < 0;
}

void Dump(const Array<BaseClass>& a)
{
	std::cout << "Dump of array\n";
	for(int i = 0; i < a.GetCount(); i++)
		std::cout << a[i].Name() << "\n";
	std::cout << "-------------\n";
}

int main()
{
	Array<BaseClass> a;
	a.Add(new BaseClass);
	a.Add(new Derived1);
	a.Add(new Derived2);
	a.Add(new BaseClass);
	a.Add(new Derived1);
	a.Add(new Derived2);
	Dump(a);
	Array<BaseClass> b;
	b <<= a;
	Dump(b);
	Sort(a);
	Dump(a);
	BaseClass *x = b.Detach(1);
	Dump(b);
	std::cout << x->Name() << "\n";
	delete x;
	return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/cont.cpp>
#endif
