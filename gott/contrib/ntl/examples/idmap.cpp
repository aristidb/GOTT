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

struct CountOrder
{
	bool operator()(const Vector<int>& a, const Vector<int>& b) const
	{
		return a.GetCount() > b.GetCount();
	}
};

int main(int argc, const char *argv[])
{
	if(argc != 2 && argc != 3) {
		std::cout << "Usage: idmap file [c]\n";
		return 1;
	}
	
	std::ifstream in(argv[1]);
	if (!in) {
		std::cout << "Cannot open input file.\n";
		return 2;
	}
	
	VectorMap< std::string, Vector<int> > map;
	int line = 1;
	
	for(;;) {
		int c = in.get();
		if(c == EOF) break;
		if(isalpha(c) || c == '_') {
			std::string id;
			id += (char)c;
			c = in.get();
			while(c != EOF && (isalnum(c) || c == '_')) {
				id += (char)c;
				c = in.get();
			}
			map.GetAdd(id).Add(line);
		}
		else
		if(isdigit(c))
			do c = in.get();
			while(c != EOF && (isalnum(c) || c == '.'));
		if(c == '\n')
			++line;
	}

	Vector<int> order;
	if(argc == 3 && argv[2][0] == 'c')
		order = GetSortOrder(map.GetValues(), CountOrder());
	else
		order = GetSortOrder(map.GetKeys());

	for(int i = 0; i < order.GetCount(); i++) {
		std::cout << map.GetKey(order[i]) << ": ";
		const Vector<int>& l = map[order[i]];
		for(int i = 0; i < l.GetCount(); i++) {
			if(i) std::cout << ", ";
			std::cout << l[i];
		}
		std::cout << '\n';
	}

	return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/cont.cpp>
#endif
