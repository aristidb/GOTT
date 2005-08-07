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
#include <fstream>
#include <time.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>

#include <ntls.h>

using namespace std;

#if !defined(__GNUC__) && !defined(__MWERKS__)
#define NOHASHMAP
#endif

#ifndef NOHASHMAP
#ifdef __GNUC__
#include <ext/hash_map>
#include <ext/hash_set>
#elif defined(__MWERKS__)
#include <hash_map>
#include <hash_set>
#else
#error where is hash_map
#endif
#endif

#ifndef NOHASHMAP
#ifdef __GNUC__
using namespace __gnu_cxx;
#elif defined(__MWERKS__)
using namespace Metrowerks;
#else
#error where is hash_map
#endif
#endif

#include <ntls.h>

using namespace std;

template <class T>
struct HashFn {
	size_t  operator() (const T& x) const { return GetHashValue (x); }
};

double TimeStop()
{
	static int clk;
	int q = clock();
	int n = q - clk;
	clk = q;
	return (double) n / CLOCKS_PER_SEC;
}

void PrintLine()
{
	cout << "-------------------------------------------------------\n";
}

struct Elem : Moveable<Elem> {
	string         text;
	vector<string> data;
};

struct NElem : Moveable<NElem> {
	string         text;
	Vector<string> data;
};

struct Key : Moveable<Key> {
	string text;
	int    number;
	
	Key(const string& text) : text(text) {}
};

inline bool operator<(const Key& a, const Key& b)
{
	return a.text < b.text;
}

int main(int argc, const char *argv[])
{
	int i, j, n;
	
	Vector<string> key;
	for(i = 0; i < 1000000; i++) {
		char h[50];
		sprintf(h, "%.6f", (double) i / 7);
		key.Add(h);
	}
	random_shuffle(key.Begin(), key.End());
	random_shuffle(key.Begin(), key.End());
	random_shuffle(key.Begin(), key.End());

	TimeStop();
	for(n = 0; n < 100; n++) {
		vector<int> v;
		for(int i = 0; i < 50000; i++)
			v.push_back(i);
	}
	cout << "Adding 50000 elements to vector<int>: " << TimeStop() * 10 << " ms\n";
	TimeStop();
	for(n = 0; n < 100; n++) {
		Vector<int> v;
		for(int i = 0; i < 50000; i++)
			v.Add(i);
	}
	cout << "Adding 50000 elements to Vector<int>: " << TimeStop() * 10 << " ms\n";
	PrintLine();
	TimeStop();
	for(n = 0; n < 10; n++) {
		vector<string> v;
		for(int i = 0; i < 50000; i++)
			v.push_back(key[i]);
	}
	cout << "Adding 50000 elements to vector<string>: " << TimeStop() * 10 << " ms\n";
	TimeStop();
	for(n = 0; n < 10; n++) {
		Vector<string> v;
		for(int i = 0; i < 50000; i++)
			v.Add(key[i]);
	}
	cout << "Adding 50000 elements to Vector<string>: " << TimeStop() * 10 << " ms\n";
	PrintLine();
	
	TimeStop();
	for(n = 0; n < 1000; n++)
	{
		vector< vector<int> > v;
		for(int i = 0; i < 100; i++) {
			vector<int> x;
			for(int q = 0; q < 100; q++)
				x.push_back(q);
			v.push_back(x);
		}
	}
	cout << "Adding 100x100 e. to vector< vector<int> >: " << TimeStop() << " ms\n";
	TimeStop();
	for(n = 0; n < 1000; n++)
	{
		vector< vector<int> > v;
		for(int i = 0; i < 100; i++) {
			v.push_back(vector<int>());
			vector<int>& w = v.back();
			for(int q = 0; q < 100; q++)
				w.push_back(q);
		}
	}
	cout << "Adding 100x100 e. to vector< vector<int> > (smart): " << TimeStop() << " ms\n";
	TimeStop();
	for(n = 0; n < 1000; n++)
	{
		Vector< Vector<int> > v;
		for(int i = 0; i < 100; i++) {
			Vector<int>& w = v.Add();
			for(int q = 0; q < 100; q++)
				w.Add(q);
		}
	}
	cout << "Adding 100x100 e. to Vector< Vector<int> >: " << TimeStop() << " ms\n";
	PrintLine();

	TimeStop();
	for(n = 0; n < 1000; n++)
	{
		vector< vector<string> > v;
		for(int i = 0; i < 100; i++) {
			vector<string> x;
			for(int q = 0; q < 100; q++)
				x.push_back(key[q]);
			v.push_back(x);
		}
	}
	cout << "Adding 100x100 e. to vector< vector<string> >: " << TimeStop() << " ms\n";
	TimeStop();
	for(n = 0; n < 1000; n++)
	{
		vector< vector<string> > v;
		for(int i = 0; i < 100; i++) {
			v.push_back(vector<string>());
			vector<string>& w = v.back();
			for(int q = 0; q < 100; q++)
				w.push_back(key[q]);
		}
	}
	cout << "Adding 100x100 e. to vector< vector<string> >: " << TimeStop() << " ms\n";
	TimeStop();
	for(n = 0; n < 1000; n++)
	{
		Vector< Vector<string> > v;
		for(int i = 0; i < 100; i++) {
			Vector<string>& w = v.Add();
			for(int q = 0; q < 100; q++)
				w.Add(key[q]);
		}
	}
	cout << "Adding 100x100 e. to Vector< Vector<string> >: " << TimeStop() << " ms\n";
	PrintLine();
	
	TimeStop();
	for(n = 0; n < 1000; n++)
	{
		vector< Elem > v;
		for(int i = 0; i < 100; i++) {
			Elem x;
			x.text = key[i];
			for(int q = 0; q < 100; q++)
				x.data.push_back(key[q]);
			v.push_back(x);
		}
	}
	cout << "Adding 100x100 e. to vector< struct { string, vector<string> } >: " << TimeStop() << " ms\n";
	TimeStop();
	for(n = 0; n < 1000; n++)
	{
		vector< Elem > v;
		for(int i = 0; i < 100; i++) {
			v.push_back(Elem());
			Elem& w = v.back();
			w.text = key[i];
			for(int q = 0; q < 100; q++)
				w.data.push_back(key[q]);
		}
	}
	cout << "Adding 100x100 e. to vector< struct { string, vector<string> } > (s.): " << TimeStop() << " ms\n";
	TimeStop();
	for(n = 0; n < 1000; n++)
	{
		Vector< NElem > v;
		for(int i = 0; i < 100; i++) {
			NElem& w = v.Add();
			w.text = key[i];
			for(int q = 0; q < 100; q++)
				w.data.Add(key[q]);
		}
	}
	cout << "Adding 100x100 e. to Vector< struct { string, Vector<string> } >: " << TimeStop() << " ms\n";
	PrintLine();

	TimeStop();
	for(n = 0; n < 1000; n++) {
		vector<string> v;
		for(int i = 0; i < 1000; i++)
			v.insert(lower_bound(v.begin(), v.end(), key[i]), key[i]);
	}
	cout << "Lower-bound inserting 1000 elements to vector<string>: " << TimeStop() << " ms\n";

	TimeStop();
	for(n = 0; n < 1000; n++) {
		Vector<string> v;
		for(int i = 0; i < 1000; i++)
			v.Insert(FindLowerBound(v, key[i]), key[i]);
	}
	cout << "Lower-bound inserting 1000 elements to Vector<string>: " << TimeStop() << " ms\n";
	PrintLine();

	TimeStop();
	for(n = 0; n < 1000; n++) {
		vector<Key> v;
		for(int i = 0; i < 1000; i++)
			v.insert(lower_bound(v.begin(), v.end(), key[i]), key[i]);
	}
	cout << "Lower-bound inserting 1000 elements to vector<struct { string, int }>: " << TimeStop() << " ms\n";

	TimeStop();
	for(n = 0; n < 1000; n++) {
		Vector<Key> v;
		for(int i = 0; i < 1000; i++)
			v.Insert(FindLowerBound(v, key[i]), key[i]);
	}
	cout << "Lower-bound inserting 1000 elements to Vector<struct { string, int }>: " << TimeStop() << " ms\n";
	PrintLine();
	
	if(argc != 2) {
		cout << argc << "Missing input file for map benchmarks !\n";
		return 0;
	}

	std::ifstream in(argv[1]);
	if (!in) {
		cout << "File " << argv[1] << " not found !\n";
		return 0;
	}
	Vector<string> line;
	string ln;
	for(;;) {
		int c = in.get();
		if(c == EOF || c == '\n') {
			line.Add(ln);
			line.Add(ln);
			line.Add(ln);
			ln = string();
			if(c == EOF) break;
		}
		else
		if(c >= ' ')
			ln += (char)c;
	}
	
	
	random_shuffle(line.Begin(), line.End());
	random_shuffle(line.Begin(), line.End());
	random_shuffle(line.Begin(), line.End());

	PrintLine();
	
	TimeStop();
	for(i = 0; i < 1000; i++) {
		Index<string> x;
		for(j = 0; j < line.GetCount(); j++)
			x.FindAdd(line[j]);
	}
	cout << "Creating set of inputfile lines using Index done in " << TimeStop() << " ms\n";
	TimeStop();
	for(i = 0; i < 1000; i++) {
		Index<string> x;
		x.Reserve(193);
		for(j = 0; j < line.GetCount(); j++)
			x.FindAdd(line[j]);
	}
	cout << "Creating set of inputfile lines using Index / Reserve(193) done in " << TimeStop() << " ms\n";
	TimeStop();
	for(i = 0; i < 1000; i++) {
		set<string> x(line.begin(), line.end());
	}
	cout << "Creating set of inputfile lines using std::set done in " << TimeStop() << " ms\n";
#ifndef NOHASHMAP
	TimeStop();
	for(i = 0; i < 1000; i++) {
		hash_set<string, HashFn<string> > x(line.begin(), line.end());
	}
	cout << "Creating set of inputfile lines using std::hash_set done in " << TimeStop() << " ms\n";
	TimeStop();
	for(i = 0; i < 1000; i++) {
		hash_set<string, HashFn<string> > x;
		for(j = 0; j < line.GetCount(); j++)
			x.insert(line[j]);
	}
	cout << "Creating set of inputfile lines using std::hash_set / incremental done in "
	     << TimeStop() << " ms\n";
#endif
	PrintLine();

	Index<string> ls;
	for(j = 0; j < line.GetCount(); j++)
		ls.FindAdd(line[j]);
	Vector<string> lineset = ls.PickKeys();
	random_shuffle(lineset.Begin(), lineset.End());
	random_shuffle(lineset.Begin(), lineset.End());
	random_shuffle(lineset.Begin(), lineset.End());

	TimeStop();
	for(i = 0; i < 1000; i++) {
		VectorMap<string, int> map;
		for(j = 0; j < line.GetCount(); j++)
			map.Add(line[j], j);
		for(j = 0; j < lineset.GetCount(); j++) {
			int q = map.Find(lineset[j]);
			int cnt = 0;
			int dummy_sum = 0;
			while(q >= 0) {
				cnt++;
				q = map.FindNext(q);
				dummy_sum += map[q];
			}
			ASSERT(cnt > 0 && cnt % 3 == 0);
		}
	}
	cout << "Multimap test - VectorMap done in " << TimeStop() << " ms\n";
	
	TimeStop();
	for(i = 0; i < 1000; i++) {
		multimap<string, int> map;
		for(j = 0; j < line.GetCount(); j++)
			map.insert(make_pair(line[i], i));
		for(j = 0; j < lineset.GetCount(); j++) {
			typedef multimap<string, int>::const_iterator iter;
			pair<iter, iter> r = map.equal_range(lineset[j]);
			iter i = r.first;
			int cnt = 0;
			int dummy_sum = 0;
			if(i != map.end())
				for(;;) {
					cnt++;
					dummy_sum += i->second;
					if(i == r.second) break;
					i++;
				}
/*	another variant - seems same...
			multimap<string, int>::const_iterator f, i;
			f = i = map.find(lineset[j]);
			int cnt = 0;
			while(i != map.end() && f->first == i->first) {
				cnt++;
				i++;
			}*/
			ASSERT(cnt > 0 && cnt % 3 == 0);
		}
	}
	cout << "Multimap test - std::multimap done in " << TimeStop() << " ms\n";

#ifndef NOHASHMAP
	TimeStop();
	for(i = 0; i < 1000; i++) {
		hash_multimap<string, int, HashFn<string> > map;
		for(j = 0; j < line.GetCount(); j++)
			map.insert(make_pair(line[i], i));
		for(j = 0; j < lineset.GetCount(); j++) {
			typedef hash_multimap<string, int, HashFn<string> >::const_iterator iter;
			pair<iter, iter> r = map.equal_range(lineset[j]);
			iter i = r.first;
			int cnt = 0;
			int dummy_sum = 0;
			if(i != map.end())
				for(;;) {
					cnt++;
					dummy_sum += i->second;
					if(i == r.second) break;
					i++;
				}
/*	another variant - seems same...
			multimap<string, int>::const_iterator f, i;
			f = i = map.find(lineset[j]);
			int cnt = 0;
			while(i != map.end() && f->first == i->first) {
				cnt++;
				i++;
			}*/
			ASSERT(cnt > 0 && cnt % 3 == 0);
		}
	}
	cout << "Multimap test - hash_multimap done in " << TimeStop() << " ms\n";
#endif

	#ifdef _WIN32
	getchar();
	#endif

	return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/cont.cpp>
#endif
