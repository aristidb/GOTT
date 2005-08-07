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
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <time.h>

#include <ntls.h>

using namespace std;

void BenchNTL(const char *file) {
	std::ifstream in(file);
	if(!in) {
		std::cout << "Cannot open input file.\n";
		return;
	}
	
	VectorMap< string, Vector<int> > map;
	int line = 1;
	
	for(;;) {
		int c = in.get();
		if(c == EOF) break;
		if(isalpha(c) || c == '_') {
			std::string id;
			id.reserve(64);
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

	Vector<int> order = GetSortOrder(map.GetKeys());
/* For benchmark purposes, output is omitted
	for(int i = 0; i < order.GetCount(); i++) {
		std::cout << map.GetKey(order[i]) << ": ";
		const Vector<int>& l = map[order[i]];
		for(int i = 0; i < l.GetCount(); i++) {
			if(i) std::cout << ", ";
			std::cout << l[i];
		}
		std::cout << '\n';
	}
*/
}

void BenchSTL(const char *file) {
	std::ifstream in(file);
	if (!in) {
		std::cout << "Cannot open input file.\n";
		return;
	}
	
	map< string, vector<int> > imap;
	int line = 1;
	
	for(;;) {
		int c = in.get();
		if(c == EOF) break;
		if(isalpha(c) || c == '_') {
			std::string id;
			id.reserve(64);
			id += (char)c;
			c = in.get();
			while(c != EOF && (isalnum(c) || c == '_')) {
				id += (char)c;
				c = in.get();
			}
			imap[id].push_back(line);
		}
		else
		if(isdigit(c))
			do c = in.get();
			while(c != EOF && (isalnum(c) || c == '.'));
		if(c == '\n')
			++line;
	}

/* For benchmark purposes, output is omitted
	map< std::string, vector<int> >::const_iterator e = imap.end();
	for(map< std::string, vector<int> >::const_iterator i = imap.begin(); i != e; i++) {
		std::cout << i->first << ": ";
		vector<int>::const_iterator e = i->second.end();
		vector<int>::const_iterator b = i->second.begin();
		for(vector<int>::const_iterator j = b; j != e; j++) {
			if(j != b) std::cout << ", ";
			std::cout << *j;
		}
		std::cout << '\n';
	}
*/
}

double TimeStop()
{
	static int clk;
	int q = clock();
	int n = q - clk;
	clk = q;
	return (double) n / CLOCKS_PER_SEC;
}

int main(int argc, const char *argv[])
{
	if(argc != 2) {
		std::cout << "Usage: idmap_bench file\n";
		return 1;
	}

	int n;

	BenchSTL(argv[1]);
	TimeStop();
	for(n = 0; n < 100; n++)
		BenchSTL(argv[1]);
	cout << "STL time: " << TimeStop() << '\n';

	BenchNTL(argv[1]);
	TimeStop();
	for(n = 0; n < 100; n++)
		BenchNTL(argv[1]);
	cout << "NTL time: " << TimeStop() << '\n';
	
	return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/cont.cpp>
#endif
