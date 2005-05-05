// This file is in the public domain.

#ifndef GOTT_UTIL_MY_HASH_MAP
#define GOTT_UTIL_MY_HASH_MAP

#ifdef __GNUC__
#define HH_HASH_MAP <ext/hash_map>
#define HH_HASHD __gnu_cxx
#define NEED_HASH_FOR_STRING
#else
#define HH_HASH_MAP <hash_map>
#endif

#ifdef _MSC_VER
#define HH_HASHD stdext
#define _DEFINE_DEPRECATED_HASH_CLASSES 0
#endif

#include HH_HASH_MAP
#include <string>

namespace hashd = HH_HASHD;

#ifdef  NEED_HASH_FOR_STRING
namespace HH_HASHD {
template<class Ch, class ChTr, class A> struct hash<std::basic_string<Ch, ChTr, A> > {
    typedef std::basic_string<Ch, ChTr, A> string_t;

    std::size_t operator() (string_t const &a) const {
        size_t x = 0;
        for (typename string_t::const_iterator it = a.begin(); it != a.end(); ++it)
            x = 5*x + *it;
        return x;
    }
};
}
#endif

#endif
