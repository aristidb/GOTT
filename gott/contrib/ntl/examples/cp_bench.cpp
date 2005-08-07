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

///////////////////////////////////////////////////////////////////////////
// Based on benchmark presented in in alt.comp.c++.moderated
// (c) Bjarne Stroustrup, Alexander Stepanov

#ifndef NDEBUG
#define NDEBUG
#endif

#include <ntls.h>

#include <stddef.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>
#include <list>
#include <deque>
#include <set>

#include <iostream>
#include <iomanip>

using namespace std;

#ifndef __GNUC__
#define NOHASHMAP
#endif

#ifndef NOHASHMAP
#include <ext/hash_set>
using namespace __gnu_cxx;
#endif

/* Uncomment to use uppalloc.cpp for global new/delete

namespace upp {
	void  Free(void *ptr);
	void *Alloc(size_t size);
};

inline void *operator new(size_t size) throw(std::bad_alloc)   { return upp::Alloc(size); }
inline void operator  delete(void *ptr)                        { upp::Free(ptr); }

inline void *operator new[](size_t size) throw(std::bad_alloc) { return upp::Alloc(size); }
inline void operator  delete[](void *ptr)                      { upp::Free(ptr); }

*/

template <class T>
struct HashFn {
	size_t  operator() (const T& x) const { return GetHashValue (x); }
};

template <class T>
void array_test(T *first, T *last)
{
	T* array = new T[last - first];
	copy(first, last, array);
	sort(array, array + (last - first));
	unique(array, array + (last - first));
	delete [] array;
}

template <class T>
void vector_pointer_test(T *first, T *last)
{ 
   vector<T> container(first, last);
   sort(&*container.begin(), &*container.end());
   unique(&*container.begin(), &*container.end());
}

template <class T>
void vector_iterator_test(T *first, T *last)
{
	vector<T> container(first, last);
	sort(container.begin(), container.end());
	unique(container.begin(), container.end());
}

template <class T>
void vector_iterator_test_inc(T *first, T *last)
{
	if(last - first > 10000)
		return;
	vector<T> r;
	while(first != last) {
		r.insert(lower_bound(r.begin(), r.end(), *first), *first);
		first++;
	}
}

template <class T>
void Vector_test_inc(T *first, T *last)
{
	if(last - first > 10000)
		return;
	Vector<T> r;
	while(first != last) {
		r.Insert(FindLowerBound(r, *first), *first);
		first++;
	}
}

template <class T>
void list_test(T* first, T* last)
{ 
	return;
	list<T> container(first, last);
	container.sort();
	container.unique();
}

template <class T>
void set_test(T *first, T *last)
{
	set<T> container(first, last);
}

template <class T>
void set_test_inc(T *first, T *last)
{
	set<T> x;
	while(first != last)
		x.insert(*first++);
}

template <class T>
void hash_set_test(T *first, T *last)
{
#ifndef NOHASHMAP
	hash_set< T, HashFn<T> > x(first, last);
#endif
}

template <class T>
void hash_set_test_inc(T *first, T *last)
{
#ifndef NOHASHMAP
	hash_set< T, HashFn<T> > x;
	while(first != last)
		x.insert(*first++);
#endif
}

template <class T>
void index_test(T *first, T *last)
{
	Index<T> ndx;
	ndx.Reserve(last - first);
	while(first != last)
		ndx.FindAdd(*first++);
}

template <class T>
void index_test_inc(T *first, T *last)
{
	Index<T> ndx;
	while(first != last)
		ndx.FindAdd(*first++);
}

clock_t start_time;

void print_time()
{
	cout << setw(10) << (clock() - start_time) / double(CLOCKS_PER_SEC);
	start_time = clock();
}

const int largest_size = 1000000;

template <class T>
void run_tests(int size, int duplicates, T* = 0) // fixed for VC++ 6
{
	int q;

	const size_t length = duplicates * size;
	
	if(length > 2000000)
		return;
	
	vector<T> buf(length);
	T *buffer = &buf[0];
	T *buffer_end = &buf[length];
	for(q = 0; q < duplicates; q++)	
		initialize(buffer + q + size, buffer + q * size + size);
	random_shuffle(buffer, buffer_end);
	random_shuffle(buffer, buffer_end);
	random_shuffle(buffer, buffer_end);

	int n = 0;
	clock_t tt = clock() + CLOCKS_PER_SEC;
	while(clock() < tt) {
		set_test(buffer, buffer_end);
		++n;
	}
	
	cout << setw(8) << setiosflags(ios::left) << size;
	cout << setw(10) << fixed << setprecision(2);

	start_time = clock();

	for(q = n; q--;)
		array_test(buffer, buffer_end);
	print_time();
	
//	for(q = n; q--;)
//		vector_pointer_test(buffer, buffer_end);
//	print_time();

//	for(q = n; q--;)
//		vector_iterator_test(buffer, buffer_end);
//	print_time();

//	for(q = n; q--;)
//		vector_iterator_test_inc(buffer, buffer_end);
//	print_time();

	for(q = n; q--;)
		Vector_test_inc(buffer, buffer_end);
	print_time();

//	for(q = n; q--;)
//		list_test(buffer, buffer_end);
//	print_time();

	for(q = n; q--;)
		set_test(buffer, buffer_end);
	print_time();

//	for(q = n; q--;)
//		set_test_inc(buffer, buffer_end);
//	print_time();
	
	for(q = n; q--;)
		hash_set_test_inc(buffer, buffer_end);
	print_time();

	for(q = n; q--;)
		hash_set_test(buffer, buffer_end);
	print_time();

	for(q = n; q--;)
		index_test_inc(buffer, buffer_end);
	print_time();

	for(q = n; q--;)
		index_test(buffer, buffer_end);
	print_time();

	cout << '\n';
}

template <class T>
void test_type(const char *name, T* = 0) // Fix for VC++ 6
{
	const int duplicates [] = { 2, 5, 30, 100 };
	for(int d = 0; d < sizeof(duplicates)/sizeof(int); d++) {
		const int sizes [] = { 10, 30, 100, 300, 500, 1000, 10000, 100000, 1000000 };
		const int n = sizeof(sizes)/sizeof(int);
		cout << name << ", " << duplicates[d] << " duplicates\n";
		cout << "size    "
		        "c-array   "
	//	        "vector/p  "
	//	        "vector/i  "
	//	        "vec-inc   "
				"Vec-inc   "
	//	        "list      "
		        "set       "
	//	        "set-inc   "
				"h_set-inc "
				"hash_set  "
		        "Index-inc "
		        "Index-R"
		        "\n";
		for(int i = 0; i < n; ++i)
	  		run_tests<T>(sizes[i], duplicates[d]);
		cout << "\n";
	}
}

void initialize(int *first, int *last)
{
	int value = 0;
	while (first != last)
		*first++ = value++;
}

void initialize(double *first, double *last)
{
	double value = 0;
	while (first != last) {
		*first++ = value;
		value += 1;
	}
}

void initialize(string *first, string *last)
{
	int i = 0;
	while (first != last) {
		char h[255];
		sprintf(h, "%.6f", (double)i++ / 7);
		*first++ = h;
	}
}

struct Struct1 : Moveable<Struct1>
{
	int a;
	int b;
	
	bool operator<(const Struct1& x) const {
		return a == x.a ? b < x.b : a < x.a;
	}

	bool operator==(const Struct1& x) const {
		return a == x.a && b == x.b;
	}
	
	friend unsigned GetHashValue(const Struct1& x) {
		return CombineHash(GetHashValue(x.a), GetHashValue(x.b));
	}
};

void initialize(Struct1 *first, Struct1 *last)
{
	srand(1);
	while (first != last) {
		first->a = rand();
		first->b = rand();
		first++;
	}
}

struct Struct2 : Moveable<Struct2>
{
	int    a;
	string b;
	
	bool operator<(const Struct2& x) const {
		return a == x.a ? b < x.b : a < x.a;
	}

	bool operator==(const Struct2& x) const {
		return a == x.a && b == x.b;
	}
	
	friend unsigned GetHashValue(const Struct2& x) {
		return CombineHash(GetHashValue(x.a), GetHashValue(x.b));
	}
};

void initialize(Struct2 *first, Struct2 *last)
{
	srand(1);
	while (first != last) {
		char h[256];
		sprintf(h, "%d", rand());
		first->a = rand();
		first->b = h;
		first++;
	}
}

int main()
{
	test_type<int>("int");
//	test_type<double>("double");
	test_type<string>("string");
//	test_type<Struct1>("struct Struct1 { int a; int b; }");
	test_type<Struct2>("struct Struct2 { int a; string b; }");
	
#ifdef _WIN32
	getchar();
#endif
	return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/cont.cpp>
#endif
