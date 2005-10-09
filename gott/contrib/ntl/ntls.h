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

#ifndef NTLS_H
#define NTLS_H

#include <string>
#include <ntl.h>

NTL_MOVEABLE(std::string)

template<>
inline unsigned GetHashValue(const std::string& x)
{
	return memhash(x.c_str(), x.size());
}

#ifndef NOWSTRING

NTL_MOVEABLE(std::wstring)

template<>
inline unsigned GetHashValue(const std::wstring& x)
{
	CombineHash hash;
	for(size_t i = 0; i != x.size(); ++i)
		hash = hash.Put(x[i]);
	return hash;
}

#endif

#endif
