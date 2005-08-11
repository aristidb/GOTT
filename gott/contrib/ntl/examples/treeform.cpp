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

// This is a NTL version of treeform example presented at
// http://www.cs.rpi.edu/~musser/stl-bigger-example.html

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <ntls.h>

using namespace std;

string ReadChunk(istream& in, char delim = '\t') {
	string txt;
	txt.reserve(20);
	int c = in.get();
	while(c != EOF && c != delim) {
		txt += (char)c;
		c = in.get();
	}
	return txt;
}

struct Person : Moveable<Person> {
	string name;
	string place;
	string year;
};

bool operator<(const Person& a, const Person& b)
{
	return a.year < b.year;
}

void Print(const VectorMap<string, Person>& gene, string advisor, int indent)
{
	int q = gene.Find(advisor);
	while(q >= 0) {
		const Person& p = gene[q];
		cout << string(indent * 8, ' ') << p.name
		     << " (" << p.place << " " << p.year << ")" << endl;
		Print(gene, p.name, indent + 1);
		q = gene.FindNext(q);
	}
}

int main(int argc, const char *argv[])
{
	if(argc != 2) {
		std::cout << "Usage: treeform file\n";
		return 1;
	}
	
	std::ifstream in(argv[1]);
	if (!in) {
		std::cout << "Cannot open input file.\n";
		return 2;
	}
	
	Vector<Person> person;
	Vector<string> advisor;
	Index<string>  pidx;
	
	for(;;) {
		string h = ReadChunk(in);
		if(in.eof()) break;
		Person& p = person.Add();
		p.name = h;
		pidx.Add(h);
		h = ReadChunk(in);
		advisor.Add(h.empty() || h == "?" ? "---" : h);
		p.place = ReadChunk(in);
		p.year = ReadChunk(in, '\n');
	}

	for(int i = 0; i < advisor.GetCount(); i++)
		if(advisor[i] != "---" && pidx.Find(advisor[i]) < 0) {
			person.Add().name = advisor[i];
			pidx.Add(advisor[i]);
		}
	advisor.SetCount(person.GetCount(), "---");

	IndexSort(person, advisor);
	VectorMap<string, Person> gene(advisor, person);
	Print(gene, "---", 0);

	return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/Cont.cpp>
#endif
