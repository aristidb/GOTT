// This file is in the public domain.

#ifndef GOTT_UTIL_MY_HASH_MAP
#define GOTT_UTIL_MY_HASH_MAP

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

namespace hashd = HH_HASHD;

namespace HH_HASHD {
  template<class> struct hash;
}

#endif
