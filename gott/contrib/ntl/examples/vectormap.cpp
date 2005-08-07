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

using namespace std;

int main()
{
	VectorMap<int, string> map;

	map.Add(0, "Zero");
	map.Add(1, "One");
	map.Add(2, "Two");
	map.Add(3, "Three");
	map.Add(4, "Four");
	map.Add(5, "Five");
	map.Add(6, "Six");
	map.Add(7, "Seven");
	map.Add(8, "Eight");
	map.Add(9, "Nine");

	for(;;) {
		cout << "Enter \"q\" to quit, or enter some digits: ";
		string text;
		cin >> text;
		if(text == "q")
			break;
		for(unsigned int i = 0; i < text.size(); i++)
			cout << map.Get(text[i] - '0', "[err]") << " ";
		cout << endl;
    }
    return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/cont.cpp>
#endif
