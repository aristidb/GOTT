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
#include <ostream>
#include <fstream>
#include <time.h>
#include <list>

#include <ntls.h>

using namespace std;

// ------------- Tutorial output utilities ----------------

void Line(int n = 40)
{
	while(n--)
		cout << '-';
	cout << '\n';
}

template <class T>
void DumpContainer(ostream& out, const T& cont)
{
	out << "{ ";
	for(int i = 0; i < cont.GetCount(); i++) {
		if(i)
			out << ", ";
		out << cont[i];
	}
	out << " }";
}

template <class T>
ostream& operator<<(ostream& out, const Vector<T>& a)
{
	DumpContainer(out, a);
	return out;
}

template <class T>
ostream& operator<<(ostream& out, const Array<T>& a)
{
	DumpContainer(out, a);
	return out;
}

template <class T>
ostream& operator<<(ostream& out, const BiVector<T>& a)
{
	DumpContainer(out, a);
	return out;
}

template <class T>
ostream& operator<<(ostream& out, const BiArray<T>& a)
{
	DumpContainer(out, a);
	return out;
}

template <class T>
ostream& operator<<(ostream& out, const Index<T>& a)
{
	DumpContainer(out, a);
	return out;
}

template <class T>
ostream& operator<<(ostream& out, const ArrayIndex<T>& a)
{
	DumpContainer(out, a);
	return out;
}

#define DUMP(x)           cout << #x << " = " << x << '\n'

// ------------------ Tutorial 1 --------------------

void Tutorial1()
{
	Vector<int> v;
	v.Add(1);
	v.Add(2);
	v.Add(3);

	for(int i = 0; i < v.GetCount(); i++)
		cout << v[i] << '\n';

	Line();

	for(Vector<int>::Iterator q = v.Begin(), e = v.End(); q != e; q++)
		cout << *q << '\n';
}

// ------------------ Tutorial 2 --------------------

int RandomValue()
{
	static int q = (rand() & 31) + 10;
	return rand() % q;
}

void Tutorial2()
{
	Vector<int> v;
	v.Add(1);
	v.Add(2);
	
	v.Insert(1, 10);
	DUMP(v);
	v.Remove(0);
	DUMP(v);
	
	srand( (unsigned)time( NULL ) );
	v.Clear();
	for(int i = 0; i < 10000; i++)
		v.At(RandomValue(), 0)++;
	DUMP(v);
	Sort(v);
	DUMP(v);
}

// ------------------ Tutorial 3 --------------------

void Tutorial3()
{
	Vector<int> v;
	v.Add(1);
	v.Add(2);

	Vector<int> v1 = v;
	DUMP(v1);
//	cout << v << '\n'; // Illegal
	v <<= v1;
	DUMP(v);
	Vector<int> v2(v, 0);
	DUMP(v2);
	DUMP(v);
}

// ------------------ Tutorial 4 --------------------

struct Distribution : Moveable<Distribution> {
	string      text;
	Vector<int> data;
};

ostream& operator<<(ostream& out, const Distribution& a)
{
	return out << a.text << ": " << a.data;
}

Distribution CreateDist(int n)
{
	Distribution d;
	char h[20];
	sprintf(h, "Test %d", n);
	d.text = h;
	for(int i = 0; i < 10000; i++)
		d.data.At(rand() % n, 0)++;
	return d;
}

void Tutorial4()
{
	int i, n;

	Vector<Distribution> dist;
	for(n = 5; n <= 10; n++) {
		Distribution& d = dist.Add();
		char h[20];
		sprintf(h, "Test %d", n);
		d.text = h;
		for(i = 0; i < 10000; i++)
			d.data.At(rand() % n, 0)++;
	}
	for(i = 0; i < dist.GetCount(); i++)
		cout << dist[i] << '\n';
		
	Line();
	
	dist.Clear();
	for(n = 5; n <= 10; n++)
		dist.AddPick(CreateDist(n));
//		dist.Add() = CreateDist(); // alternative
	for(i = 0; i < dist.GetCount(); i++)
		cout << dist[i] << '\n';
}

// ------------------ Tutorial 5 --------------------

ostream& operator<<(ostream& out, const list<int>& a)
{
	out << "{ ";
	for(list<int>::const_iterator i = a.begin(); i != a.end(); i++) {
		if(i != a.begin())
			out << ", ";
		out << *i;
	}
	out << " }";
	return out;
}

void Tutorial5()
{
	Array< std::list<int> > al;
	for(int i = 0; i < 4; i++) {
		std::list<int>& l = al.Add();
		for(int q = 0; q < i; q++)
			l.push_back(q);
	}
	DUMP(al);
}

// ------------------ Tutorial 6 --------------------

struct Number {
	virtual double Get() const = 0;
};

ostream& operator<<(ostream& out, const Number& n)
{
	return out << n.Get();
}

bool operator<(const Number& a, const Number& b)
{
	return a.Get() < b.Get();
}

struct Integer : public Number {
	int n;
	virtual double Get() const { return n; }
	
	Integer(int n) : n(n) {}
};

struct Double : public Number {
	double n;
	virtual double Get() const { return n; }
	
	Double(double n) : n(n) {}
};

void Tutorial6()
{
	Array<Number> num;
	num.Add(new Integer(3));
	num.Add(new Double(15.5));
	num.Add(new Double(2.23));
	num.Add(new Integer(2));
	num.Add(new Integer(20));
	num.Add(new Double(-2.333));
	DUMP(num);
	Sort(num);
	DUMP(num);
}

// ------------------ Tutorial 7 --------------------

void Tutorial7()
{
	BiVector<int> n;
	n.AddHead(1);
	n.AddTail(2);
	n.AddHead(3);
	n.AddTail(4);
	DUMP(n);
	n.DropHead();
	DUMP(n);
	n.DropTail();
	DUMP(n);
	
	BiArray<Number> num;
	num.AddHead(new Integer(3));
	num.AddTail(new Double(15.5));
	num.AddHead(new Double(2.23));
	num.AddTail(new Integer(2));
	num.AddHead(new Integer(20));
	num.AddTail(new Double(-2.333));
	DUMP(num);
}

// ------------------ Tutorial 8 --------------------

void Tutorial8()
{
	Index<string> ndx;
	ndx.Add("alfa");
	ndx.Add("beta");
	ndx.Add("gamma");
	ndx.Add("delta");
	ndx.Add("kappa");

	DUMP(ndx);	
	DUMP(ndx.Find("beta"));
	
	DUMP(ndx.Find("something"));
	
	Line();
	
	ndx.Set(0, "delta");
	DUMP(ndx);
	
	Line();

	int fi = ndx.Find("delta");
	while(fi >= 0) {
		cout << fi << " ";
		fi = ndx.FindNext(fi);
	}
	cout << '\n';
	
	Line();
	
	DUMP(ndx.FindAdd("one"));
	DUMP(ndx.FindAdd("two"));
	DUMP(ndx.FindAdd("three"));
	DUMP(ndx.FindAdd("two"));
	DUMP(ndx.FindAdd("three"));
	DUMP(ndx.FindAdd("one"));
	
	Line();
	
	ndx.Unlink(2);
	ndx.UnlinkKey("kappa");
	DUMP(ndx.Find(ndx[2]));
	DUMP(ndx.Find("kappa"));
	DUMP(ndx.IsUnlinked(1));
	DUMP(ndx.IsUnlinked(2));
	ndx.Put("foo");
	DUMP(ndx);
	DUMP(ndx.Find("foo"));
	ndx.Sweep();
	DUMP(ndx);
	
	ndx.Remove(1);
	DUMP(ndx);
	ndx.RemoveKey("two");
	DUMP(ndx);
	ndx.Insert(0, "insert");
	DUMP(ndx);
	DUMP(ndx.Find("insert"));

	Line();
	
	Vector<string> d = ndx.PickKeys();
	Sort(d);
	DUMP(d);
}

// ------------------ Tutorial 9 --------------------

struct Person : Moveable<Person> {
	string name;
	string surname;
	
	Person(string name, string surname) : name(name), surname(surname) {}
	Person() {}
};

ostream& operator<<(ostream& out, const Person& p)
{
	return out << p.name << " " << p.surname;
}

unsigned GetHashValue(const Person& p)
{
	return CombineHash(GetHashValue(p.name), GetHashValue(p.surname));
}

bool operator==(const Person& a, const Person& b)
{
	return a.name == b.name && a.surname == b.surname;
}

unsigned GetHashValue(const Number& n)
{
	return GetHashValue(n.Get());
}

bool operator==(const Number& a, const Number& b)
{
	return a.Get() == b.Get();
}

void Tutorial9()
{
	Index<Person> p;
	p.Add(Person("John", "Smith"));
	p.Add(Person("Paul", "Carpenter"));
	p.Add(Person("Carl", "Engles"));
	      
	DUMP(p.Find(Person("Paul", "Carpenter")));
	
	ArrayIndex<Number> n;
	n.Add(new Integer(100));
	n.Add(new Double(10.5));
	n.Add(new Integer(200));
	n.Add(new Double(20.5));
	
	DUMP(n);
	DUMP(n.Find(Double(10.5)));
}

// ------------------ Tutorial 10 --------------------

bool operator<(const Person& a, const Person& b)
{
	return a.surname == b.surname ? a.name < b.name : a.surname < b.surname;
}

void Tutorial10()
{
	int i;
	VectorMap<string, Person> m;
	
	m.Add("1", Person("John", "Smith"));
	m.Add("2", Person("Carl", "Engles"));

	Person& p = m.Add("3");
	p.name = "Paul";
	p.surname = "Carpenter";
	
	DUMP(m.GetKeys());
	DUMP(m.GetValues());
	
	for(i = 0; i < m.GetCount(); i++)
		cout << m.GetKey(i) << ": " << m[i] << '\n';
	
	Line();
	
	DUMP(m.Find("2"));
	DUMP(m.Get("2"));
	DUMP(m("1"));
//	DUMP(p.Get("33")); // illegal
	DUMP(m.Get("33", Person("unknown", "person")));
	
	Line();

	m.Unlink(1);
	DUMP(m.Find("2"));
	
	m.SetKey(1, "33");
	DUMP(m.Get("33", Person("unknown", "person")));

	Line();
	
	m.Add("33", Person("Peter", "Pan"));
	DUMP(m.GetKeys());
	DUMP(m.GetValues());
	int q = m.Find("33");
	while(q >= 0) {
		cout << m[q] << '\n';
		q = m.FindNext(q);
	}
	
	Line();
	
	m.UnlinkKey("33");
	m.Put("22", Person("Ali", "Baba"));
	m.Put("44", Person("Ivan", "Wilks"));
	DUMP(m.GetKeys());
	DUMP(m.GetValues());
	
	Vector<int> order = GetSortOrder(m.GetValues());
	DUMP(order);
	for(i = 0; i < order.GetCount(); i++)
		cout << m.GetKey(order[i]) << ": " << m[order[i]] << '\n';
	
	Line();
	
	Vector<Person> ps = m.PickValues();
	DUMP(ps);
	
	Line();

	ArrayMap<string, Number> am;
	am.Add("A", new Integer(1));
	am.Add("B", new Double(2.0));
	DUMP(am.GetKeys());
	DUMP(am.GetValues());
	DUMP(am.Get("A"));
	DUMP(am.Find("B"));
}

// --------------------------------------------------

int main(int, char **)
{
	void (*tutorial[])() = {
		Tutorial1, Tutorial2, Tutorial3, Tutorial4, Tutorial5,
		Tutorial6, Tutorial7, Tutorial8, Tutorial9, Tutorial10,
		NULL
	};
	
	for(int i = 0; tutorial[i]; i++) {
		if(i) Line(60);
		char h[200];
		sprintf(h, "Tutorial %d", i + 1);
		cout << h << '\n';
		Line(12);
		(*tutorial[i])();
	}

#ifdef _WIN32
	getchar();
#endif
	return 0;
}

#ifndef NO_CONT_CPP_INCLUDE
#include <src/Cont.cpp>
#endif
