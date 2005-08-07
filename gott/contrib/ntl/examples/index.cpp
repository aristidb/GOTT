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
	Index<string> ndx;

	const char *digits[] = {
		"zero", "one", "two", "three", "four",
		"five", "six", "seven", "eight", "nine"
	};
	
	for(int i = 0; i < 10; i++)
		ndx.Add(digits[i]);

	for(;;) {
		cout << "Enter \"q\" to quit, or "
		        "enter some digits as words: ";
		string text;
		cin >> text;
		if(text == "q")
			break;
		cout << ndx.Find(text) << " " << endl;
    }
    return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/cont.cpp>
#endif
