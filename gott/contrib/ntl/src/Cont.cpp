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

#include <ntl.h>

#ifdef _MSC_VER
#pragma inline_depth(255)
#pragma optimize("t", on)
#endif

int Pow2Bound(int i)
{
	int n = 1;
	while(n < i) {
		n <<= 1;
		ASSERT(n);
	}
	return n;
}

void HashBase::Free() const
{
	if(map)
		NTL_RAW_FREE(map, mask + 1, sizeof(int));
	map = NULL;
	mask = 0;
}

void HashBase::ClearIndex() const
{
	link.Clear();
	unlinked = -1;
	Free();
}

HashBase::HashBase()
{
	unlinked = -1;
	map = NULL;
	mask = 0;
}

HashBase::HashBase(pick_ HashBase& b)
: hash(b.hash),
  link(b.link)
{
	map = b.map;
	mask = b.mask;
	unlinked = b.unlinked;
	b.map = NULL;
}

void HashBase::operator=(pick_ HashBase& b)
{
	hash = b.hash;
	link = b.link;
	Free();
	map = b.map;
	mask = b.mask;
	unlinked = b.unlinked;
	b.map = NULL;
}

HashBase::HashBase(const HashBase& b, int)
: hash(b.hash, 0)
{
	unlinked = -1;
	map = NULL;
	mask = 0;
}

void HashBase::operator<<=(const HashBase& b)
{
	ClearIndex();
	hash <<= b.hash;
}

HashBase::~HashBase()
{
	Free();
}

inline void HashBase::LinkBefore(int i, Link& l, int bi) const
{
	Link& bl = link[bi];
	l.next = bi;
	l.prev = bl.prev;
	bl.prev = i;
	link[l.prev].next = i;
}

void HashBase::Trim(int count)
{
	ASSERT(count <= hash.GetCount() && count >= 0);
	if(map) {
		for(int i = count; i < link.GetCount(); i++)
			Unlink(i, link[i]);
		link.SetCount(count);
	}
	hash.SetCount(count);
}

void HashBase::Drop(int n)
{
	Trim(hash.GetCount() - n);
}

//inline //!! Fidler: either inline or .cpp
void HashBase::FinishIndex() const
{
	int q = link.GetCount();
	link.Reserve(hash.GetAlloc());
	link.AddN(hash.GetCount() - q);
	for(int i = q; i < hash.GetCount(); i++)
		LinkTo(i, link[i], hash[i] & UNSIGNED_HIBIT ? unlinked : Mapi(i));
}

void HashBase::Reindex(int n) const
{
	ClearIndex();
	Free();
	n = ntl_max(16, Pow2Bound(n));
	map = (int *)NTL_RAW_ALLOC(n * sizeof(int));
	Fill(map, map + n, -1);
	mask = n - 1;
	FinishIndex();
}

void HashBase::Reindex() const
{
	Reindex(hash.GetCount());
}

void HashBase::Add(unsigned _hash)
{
	hash.Add(_hash & ~UNSIGNED_HIBIT);
}

void  HashBase::SetUn(int i, unsigned _hash)
{
	if(map) {
		if(link.GetCount() < hash.GetCount())
			DoIndex();
		Link& lnk = link[i];
		Unlink(i, lnk);
		LinkTo(i, lnk, Maph(_hash & ~UNSIGNED_HIBIT));
	}
	hash[i] = _hash & ~UNSIGNED_HIBIT;
}

Vector<int> HashBase::GetUnlinked() const
{
	if(link.GetCount() < hash.GetCount())
		DoIndex();
	Vector<int> r;
	int q = unlinked;
	if(q >= 0) {
		do {
			r.Add(q);
			q = link[q].next;
		}
		while(q != unlinked);
	}
	return r;
}

int HashBase::Put(unsigned _hash)
{
	if(unlinked < 0) return -1;
	if(link.GetCount() < hash.GetCount())
		DoIndex();
	Link& l = link[unlinked];
	int i = unlinked;
	unlinked = link[unlinked].next;
	if(i == unlinked)
		unlinked = -1;
	else {
		link[l.next].prev = l.prev;
		link[l.prev].next = l.next;
	}
	LinkTo(i, l, Maph(_hash & ~UNSIGNED_HIBIT));
	hash[i] = _hash & ~UNSIGNED_HIBIT;
	return i;
}

void HashBase::Set(int i, unsigned _hash) {
	if(map) {
		if(link.GetCount() < hash.GetCount())
			DoIndex();
		Link& lnk = link[i];
		Unlink(i, lnk);
		int& mi = Maph(_hash & ~UNSIGNED_HIBIT);
		int ii = mi;
		if(ii < 0)
			mi = lnk.prev = lnk.next = i;
		else
		if(i < ii) {
			LinkBefore(i, lnk, ii);
			mi = i;
		}
		else {
			int l = ii;
			int h = link[ii].prev;
			if(h - i < i - l) {
				while(i < h) {
					h = link[h].prev;
				}
				LinkBefore(i, lnk, link[h].next);
			}
			else {
				l = link[l].next;
				while(i > l && l != ii) {
					l = link[l].next;
				}
				LinkBefore(i, lnk, l);
			}
		}
	}
	hash[i] = _hash & ~UNSIGNED_HIBIT;
}

void HashBase::Shrink() {
	hash.Shrink();
	if(Pow2Bound(hash.GetCount()) != mask + 1)
		ClearIndex();
	else
		link.Shrink();
}

void HashBase::Reserve(int n)
{
	hash.Reserve(n);
	link.Reserve(n);
	n = Pow2Bound(n);
	if(n > mask + 1)
		Reindex(n);
}

void HashBase::Remove(int i)
{
	hash.Remove(i);
	ClearIndex();
}

void HashBase::Remove(const int *sorted_list, int count)
{
	hash.Remove(sorted_list, count);
	ClearIndex();
}

void HashBase::Insert(int i, unsigned _hash) {
	hash.Insert(i, _hash & ~UNSIGNED_HIBIT);
	ClearIndex();
}

#ifdef UPP
void HashBase::Serialize(Stream& s) {
	int version = 0;
	s / version;
	if(s.IsLoading())
		ClearIndex();
	hash.Serialize(s);
}
#endif

void HashBase::Swap(HashBase& b) {
	::Swap(hash, b.hash);
	::Swap(link, b.link);
	::Swap(map, b.map);
	::Swap(mask, b.mask);
	::Swap(unlinked, b.unlinked);
}


#ifdef CPU_IA32

unsigned memhash(const void *ptr, size_t count)
{
	unsigned hash = 1234567890U;

	const unsigned *ds = (unsigned *)ptr;
	const unsigned *de = ds + (count >> 2);
	while(ds < de)
		hash = ((hash << 5) - hash) ^ *ds++;

	const byte *s = (byte *)ds;
	const byte *e = s + (count & 3);
	while(s < e)
		hash = ((hash << 5) - hash) ^ *s++;

	return hash;
}

#else

unsigned memhash(const void *ptr, size_t count)
{
	unsigned hash = 1234567890U;

	const byte *s = (byte *)ptr;
	const byte *e = s + count;
	while(s < e)
		hash = ((hash << 5) - hash) ^ *s++;

	return hash;
}

#endif

unsigned GetHashValue0(const double& d)
{
	return memhash(&d, sizeof(double));
}
