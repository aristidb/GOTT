// This file is in the public domain.

#ifndef GOTT_UTIL_MY_HASH_MAP
#define GOTT_UTIL_MY_HASH_MAP

#ifdef __GNUC__
#define HH_HASH_MAP <ext/hash_map>
  #if (__GNUC__ < 3) || (__GNUC__ == 3 && __GNUC_MINOR__ == 0)
    #define HH_HASHD std
  #else
    #define HH_HASHD __gnu_cxx
  #endif
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

#ifdef NEED_HASH_FOR_STRING
namespace HH_HASHD {
template<class C,class CT,class A> struct hash<std::basic_string<C,CT,A> > {
  typedef std::basic_string<C,CT,A> string_t;

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
