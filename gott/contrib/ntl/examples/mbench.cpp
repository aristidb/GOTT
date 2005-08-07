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
#include <iomanip>
#include <fstream>
#include <time.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>

#include <ntls.h>

using namespace std;

#ifndef __GNUC__
#define NOHASHMAP
#endif

#ifndef NOHASHMAP
using namespace __gnu_cxx;
#include <ext/hash_map>
#include <ext/hash_set>
#endif

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

template <class T>
void benchmark_VectorMap(const Vector<T>& data)
{
	VectorMap<T, int> m;
	for(int i = 0; i < data.GetCount(); i++)
		m.GetAdd(data[i], 0)++;
}

template <class T>
void benchmark_VectorMapR(const Vector<T>& data)
{
	VectorMap<T, int> m;
	m.Reserve(193);
	for(int i = 0; i < data.GetCount(); i++)
		m.GetAdd(data[i], 0)++;
}

template <class T>
void benchmark_VectorMapSort(const Vector<T>& data)
{
	VectorMap<T, int> m;
	for(int i = 0; i < data.GetCount(); i++)
		m.GetAdd(data[i], 0)++;
	Vector<int> so = GetSortOrder(m.GetValues());
}

template <class C, class T>
void benchmark_map(const Vector<T>& data)
{
	C map;
	for(int i = 0; i < data.GetCount(); i++)
		map[data[i]]++;
}

template <class C>
struct map_valueorder {
	bool operator()(const typename C::const_iterator& a, const typename C::const_iterator& b) {
		return a->second < b->second;
	}
};

template <class C, class T>
void benchmark_map_sort(const Vector<T>& data)
{
	C map;
	for(int i = 0; i < data.GetCount(); i++)
		map[data[i]]++;
	vector<TYPENAME C::const_iterator> so(map.size());
	vector<TYPENAME C::const_iterator>::iterator o = so.begin();
	for(TYPENAME C::const_iterator it = map.begin(), e = map.end(); it != e; it++)
		*o++ = it;
	sort(so.begin(), so.end(), map_valueorder<C>());
}

void GenerateData(int& t, int mn) {
	t = rand() % mn;
}

void GenerateData(string& t, int mn) {
	char h[200];
	sprintf(h, "%d", rand() % mn);
	t = h;
}

template <class T>
Vector<T> Generate(int count, int d)
{
	srand(0);
	Vector<T> data;
	data.SetCount(count);
	for(int i = 0; i < count; i++)
		GenerateData(data[i], d);
	return data;
}

template <class T>
double TestVectorMap(int count, int d, int loop)
{
	Vector<T> data = Generate<T>(count, d);
	TimeStop();
	for(int i = 0; i < loop; i++)
		benchmark_VectorMap(data);
	return TimeStop();
}

template <class T>
double TestVectorMapR(int count, int d, int loop)
{
	Vector<T> data = Generate<T>(count, d);
	TimeStop();
	for(int i = 0; i < loop; i++)
		benchmark_VectorMapR(data);
	return TimeStop();
}

template <class T>
double TestVectorMapSort(int count, int d, int loop)
{
	Vector<T> data = Generate<T>(count, d);
	TimeStop();
	for(int i = 0; i < loop; i++)
		benchmark_VectorMapSort(data);
	return TimeStop();
}

template <class T, class C>
double Test_map(int count, int d, int loop)
{
	Vector<T> data = Generate<T>(count, d);
	TimeStop();
	for(int i = 0; i < loop; i++)
		benchmark_map<C>(data);
	return TimeStop();
}

template <class T, class C>
double Test_map_sort(int count, int d, int loop)
{
	Vector<T> data = Generate<T>(count, d);
	TimeStop();
	for(int i = 0; i < loop; i++)
		benchmark_map_sort<C>(data);
	return TimeStop();
}

template <class T>
void TestType(const char *t) {
	int c;
	static int dis[] = { 10, 100, 1000, 5000, 10000 };
	static int cnt[] = { 30, 100, 300, 1000, 3000, 10000, 30000, 100000 };
	for(int d = 0; d < sizeof(dis) / sizeof(int); d++) {
		cout << t << ", " << dis[d] << " distinct values\n";
		int loop[sizeof(cnt) / sizeof(int)];
		for(c = 0; c < sizeof(cnt) / sizeof(int); c++) {
			loop[c] = 0;
			double x = 0;
			while(x < 1) {
				x += TestVectorMap<T>(cnt[c], dis[d], 1);
				loop[c]++;
			}
		}
		cout << "        ";
		for(c = 0; c < sizeof(cnt) / sizeof(int); c++)
			cout << setw(8) << cnt[c];
		cout << '\n';
		cout << "V.Map   ";
		for(c = 0; c < sizeof(cnt) / sizeof(int); c++)
			cout << setw(8) <<
			        TestVectorMap<T>(cnt[c], dis[d], loop[c]);
		cout << '\n';
		cout << "map     ";
		for(c = 0; c < sizeof(cnt) / sizeof(int); c++)
			cout << setw(8) <<
			        Test_map< T, map<T, int> >(cnt[c], dis[d], loop[c]);
		cout << '\n';
	#ifndef NOHASHMAP
		cout << "h.map   ";
		for(c = 0; c < sizeof(cnt) / sizeof(int); c++)
			cout << setw(8) <<
			        Test_map< T, hash_map<T, int, HashFn<T> > >(cnt[c], dis[d], loop[c]);
		cout << '\n';
	#endif
		cout << '\n';
		cout << "V.Map/s ";
		for(c = 0; c < sizeof(cnt) / sizeof(int); c++)
			cout << setw(8) <<
			        TestVectorMapSort<T>(cnt[c], dis[d], loop[c]);
		cout << '\n';
		cout << "map/s   ";
		for(c = 0; c < sizeof(cnt) / sizeof(int); c++)
			cout << setw(8) <<
			        Test_map_sort< T, map<T, int> >(cnt[c], dis[d], loop[c]);
		cout << '\n';
	#ifndef NOHASHMAP
		cout << "h.map/s ";
		for(c = 0; c < sizeof(cnt) / sizeof(int); c++)
			cout << setw(8) <<
			        Test_map_sort< T, hash_map<T, int, HashFn<T> > >(cnt[c], dis[d], loop[c]);
		cout << '\n';
	#endif
		cout << "\n\n";
	}
}

int main()
{
	TestType<int>("int");
	TestType<string>("string");
		
#ifdef _WIN32
	getchar();
#endif
	return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/cont.cpp>
#endif
