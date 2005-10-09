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

#include <stddef.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define LLOG(x)
#define LTIMING(x)
#define ASSERT(x) assert(x)

namespace upp
{

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int   dword;

struct BlockInfo {
	BlockInfo *link;
};

struct PageLink {
	PageLink  *bucket_next;
	PageLink  *bucket_prev;
	PageLink  *free_next;
	PageLink  *free_prev;

	void LinkSelf() {
		bucket_next = bucket_prev = free_next = free_prev = this;
	}
	void UnlinkBucket() {
		bucket_prev->bucket_next = bucket_next;
		bucket_next->bucket_prev = bucket_prev;
		bucket_next = bucket_prev = this;
	}
	void InsertBucket(PageLink *lnk) {
		lnk->bucket_prev = this;
		lnk->bucket_next = bucket_next;
		bucket_next->bucket_prev = lnk;
		bucket_next = lnk;
	}
	void UnlinkFree() {
		free_prev->free_next = free_next;
		free_next->free_prev = free_prev;
		free_next = free_prev = this;
	}
	void InsertFree(PageLink *lnk) {
		lnk->free_prev = this;
		lnk->free_next = free_next;
		free_next->free_prev = lnk;
		free_next = lnk;
	}
};

struct PageInfo : public PageLink {
	BlockInfo *freelist;
	byte      *memory;
	word       freecount;
	word       blockcount;
	word       magnitude;
	word       size;
	
	void Format(int m) {
		LTIMING("Format");
		ASSERT(m >= 0 && m <= 128);
		magnitude = word(m);
		size = word(4 * m);
		byte *p = memory;
		byte *e = memory + 2048 - size;
		blockcount = 0;
		freelist = NULL;
		while(p <= e) {
			((BlockInfo *)p)->link = freelist;
			freelist = (BlockInfo *)p;
			p += size;
			blockcount++;
		}
		freecount = blockcount;
	}
};


typedef PageInfo *MemBook[512];

MemBook  nullbook[1];
MemBook *memmap[2048];

PageInfo *NewPage()
{
	LTIMING("NewPage");
	LLOG("Acquiring new page");
	static int ci = 32;
	static PageInfo *chunk;
	if(ci >= 32) {
		LLOG("**** Acquiring new 64KB block");
		dword ptr = (dword)malloc(33 * 2048);
		if(!ptr) return 0;
		dword a = (ptr + 2047) & ~2047;
		if(a - ptr >= 1024)
			chunk = (PageInfo *)ptr;
		else
			chunk = (PageInfo *)(a + 32 * 2048);
		byte *p = (byte *)a;
		for(int i = 0; i < 32; i++) {
			chunk[i].LinkSelf();
			chunk[i].memory = p;
			p += 2048;
		}
		ci = 0;
	}
	PageInfo *p = chunk + ci++;
	int pgi = ((dword)p->memory >> 11);
	int bi = pgi >> 9;
	pgi &= 511;
	if(memmap[bi] == nullbook) {
		memmap[bi] = (MemBook *)malloc(sizeof(MemBook));
		memset(memmap[bi], 0, sizeof(MemBook));
	}
	(*memmap[bi])[pgi] = p;
	return p;
}


#define InitLink2(a, n)         { a + n, a + n, a + n, a + n }, \
                                { a + n + 1, a + n + 1, a + n + 1, a + n + 1 }
#define InitLink4(a, n)         InitLink2(a, n),   InitLink2(a,   n + 2)
#define InitLink8(a, n)         InitLink4(a, n),   InitLink4(a,   n + 4)
#define InitLink16(a, n)        InitLink8(a, n),   InitLink8(a,   n + 8)
#define InitLink32(a, n)        InitLink16(a, n),  InitLink16(a,  n + 16)
#define InitLink64(a, n)        InitLink32(a, n),  InitLink32(a,  n + 32)

PageLink bucket[129] = {
	InitLink64(bucket, 0),
	InitLink64(bucket, 64),
	{ bucket + 128, bucket + 128, bucket + 128, bucket + 128 }
};

PageLink free_bucket[129] = {
	InitLink64(free_bucket, 0),
	InitLink64(free_bucket, 64),
	{ free_bucket + 128, free_bucket + 128, free_bucket + 128, free_bucket + 128 }
};

PageLink emptylist[1] = {
	{ emptylist, emptylist, emptylist, emptylist }
};

byte size_magnitude[513];


void GenerateMagnitudeTable(int *s)
{
	int size = 0;
	int magnitude = *s / 4;
	while(size <= 512) {
		ASSERT(*s);
		if(size > *s) {
			s++;
			magnitude = *s / 4;
		}
		size_magnitude[size] = byte(magnitude);
		size++;
	}
	ASSERT(*s == 512);
}


void *Alloc(size_t size)
{
	LTIMING("Alloc");
	LLOG("Alloc(" << size << ")");
	if(size <= 512) {
		int m = size_magnitude[size];
		PageLink& bm = bucket[m];
		PageInfo *page = (PageInfo *)bm.bucket_next;
		BlockInfo *q;
		if(page != &bm) {
			ASSERT(page->freecount > 0 && page->freecount < page->blockcount);
			LLOG("Allocating from page " << Dump(page));
			q = page->freelist;
			page->freelist = q->link;
			if(--page->freecount == 0) {
				LLOG("Page exhausted " << Dump(page));
				page->UnlinkBucket();
			}
			return q;
		}
		page = (PageInfo *)free_bucket[m].bucket_next;	
		if(page != &free_bucket[m]) {
			LLOG("Next empty bucket page " << Dump(page));
			ASSERT(page->freecount == page->blockcount);
			page->UnlinkFree();
			page->UnlinkBucket();
			bm.InsertBucket(page);
			q = page->freelist;
			page->freelist = q->link;
			if(--page->freecount == 0)
				page->UnlinkBucket();
			return q;
		}
		page = (PageInfo *)emptylist->free_next;
		if(page != emptylist) {
			ASSERT(page->freecount == page->blockcount);
			LLOG("Recycling page " << Dump(page));
			page->UnlinkFree();
			page->UnlinkBucket();
		}
		else {
			static bool init;
			if(!init) {
				init = true;
				static int mtable1[] = { 4, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112,
			                             128, 144, 168, 200, 256, 336, 408, 512 };
				GenerateMagnitudeTable(mtable1);
				for(int i = 0; i < 2048; i++)
					memmap[i] = nullbook;
				m = size_magnitude[size];
			}
			page = NewPage();
			if (page == 0) return 0;

		}
		page->Format(m);
		LLOG("Formated page " << Dump(page));
		bucket[m].InsertBucket(page);
		q = page->freelist;
		page->freelist = q->link;
		page->freecount--;
		return q;
	}
	void *q = malloc(size);
	return q;
}


void Free(void *ptr)
{
	int pgi = ((dword)ptr >> 11);
	PageInfo *page = (*memmap[pgi >> 9])[pgi & 511];
	if(page) {
		ASSERT(((dword)ptr & 2047) % page->size == 0);
		BlockInfo *b = (BlockInfo *)ptr;
		b->link = page->freelist;
		page->freelist = b;
		if(page->bucket_next == page) {
			LLOG("Putting to bucket " << Dump(page));
			bucket[page->magnitude].InsertBucket(page);
		}
		LLOG("Freeing in page " << Dump(page));
		if(++page->freecount == page->blockcount) {
			page->UnlinkBucket();
			free_bucket[page->magnitude].InsertBucket(page);
			emptylist->InsertFree(page);
			LLOG("Moving to empty " << Dump(page));
		}
		ASSERT(page->freecount <= page->blockcount);
	}
	else
		free(ptr);
}

}//namespace upp

#undef LLOG
#undef LTIMING
