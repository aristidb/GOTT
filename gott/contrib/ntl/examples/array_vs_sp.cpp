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

#ifndef NDEBUG
#define NDEBUG
#endif

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <vector>

#include <ntls.h>

#include <boost/smart_ptr.hpp>

using namespace std;
using namespace boost;

double TimeStop()
{
	static int clk;
	int q = clock();
	int n = q - clk;
	clk = q;
	return (double) n / CLOCKS_PER_SEC;
}

struct Base {
	virtual int Get() { return 0; }
	virtual ~Base() {}
};

struct Derived1 : Base {
	virtual int Get() { return 2; }
};

struct Derived2 : Base {
	virtual int Get() { return 3; }
};

int main()
{
	int i;
	TimeStop();
	{
		Array<Base> b;
		for(i = 0; i < 100000; i++) {
			b.Add(new Base);
			b.Add(new Derived1);
			b.Add(new Derived2);
		}
		int sum = 0;
		for(i = 0; i < 100000; i++)
			sum += b[i].Get();
	}
	cout << "Array " << TimeStop() << " s\n";
	
	TimeStop();
	{
		vector< shared_ptr<Base> > b;
		for(i = 0; i < 100000; i++) {
			b.push_back(shared_ptr<Base>(new Base));
			b.push_back(shared_ptr<Base>(new Derived1));
			b.push_back(shared_ptr<Base>(new Derived2));
		}
		int sum = 0;
		for(i = 0; i < 100000; i++)
			sum += b[i]->Get();
	}
	cout << "vector/shared_ptr " << TimeStop() << " s\n";

	return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/cont.cpp>
#endif
