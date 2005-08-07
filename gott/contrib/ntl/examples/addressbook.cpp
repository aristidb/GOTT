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

struct Info : Moveable<Info> {
	string tel;
	string email;
};

string Read(const char *prompt)
{
	cout << prompt;
	string txt;
	cin >> txt;
	return txt;
}

int main()
{
	VectorMap<string, Info> book;
	for(;;) {
		string txt = Read("Enter \"s\" to search, \"a\" to add,"
		                  " \"r\" to remove, \"q\" to quit: ");
		if(txt == "q")
			break;
		if(txt == "s") {
			txt = Read("Search: ");
			int q = book.Find(txt);
			if(q < 0)
				cout << "Not found.\n";
			else {
				Info& f = book[q];
				cout << "tel:   " << f.tel << "\n"
				     << "email: " << f.email << "\n";
			}
		}
		else
		if(txt == "a") {
			txt = Read("Name: ");
			if(book.Find(txt) >= 0)
				cout << "Name is already present.\n";
			else {
				Info& f = book.Put(txt);
				f.tel = Read("tel: ");
				f.email = Read("email: ");
			}
		}
		else
		if(txt == "r") {
			txt = Read("Remove: ");
			if(!book.UnlinkKey(txt))
				cout << "Not found.\n";
		}	
	}
	return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/cont.cpp>
#endif
