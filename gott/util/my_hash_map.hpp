// This file is in the public domain.

#if defined(PARAM_HASH_NO_BASIC_STRING) \
    ? !defined(GOTT_UTIL_MY_HASH_MAP_NO_BASIC_STRING) \
    : !defined(GOTT_UTIL_MY_HASH_MAP)
#ifndef PARAM_HASH_NO_BASIC_STRING
#define GOTT_UTIL_MY_HASH_MAP
#endif
#define GOTT_UTIL_MY_HASH_MAP_NO_BASIC_STRING

#ifdef __GNUC__
  #define HH_HASH_MAP <ext/hash_map>
  #define HH_HASH_SET <ext/hash_set>
  #if (__GNUC__ < 3) || (__GNUC__ == 3 && __GNUC_MINOR__ == 0)
    #define HH_HASHD std
  #else
    #define HH_HASHD __gnu_cxx
  #endif
  #define NEED_HASH_FOR_STRING
#else
  #define HH_HASH_MAP <hash_map>
  #define HH_HASH_SET <hash_set>
#endif

#ifdef _MSC_VER
  #define HH_HASHD stdext
  #define _DEFINE_DEPRECATED_HASH_CLASSES 0
#endif

#ifndef PARAM_HASH_NO_BASIC_STRING
#include <string>
#endif

namespace hashd = HH_HASHD;

namespace HH_HASHD {
  template<class> struct hash;
}

#if !defined(PARAM_HASH_NO_BASIC_STRING) && defined(NEED_HASH_FOR_STRING)
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
