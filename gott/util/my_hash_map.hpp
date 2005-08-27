#ifndef GOTT_UTIL_MY_HASH_MAP
#define GOTT_UTIL_MY_HASH_MAP

#ifdef __GNUC__
  #define GOTT_HASH_MAP <ext/hash_map>
  #define GOTT_HASH_SET <ext/hash_set>
  #if (__GNUC__ < 3) || (__GNUC__ == 3 && __GNUC_MINOR__ == 0)
    #define GOTT_NSHASH std
  #else
    #define GOTT_NSHASH __gnu_cxx
  #endif
#else
  #define GOTT_HASH_MAP <hash_map>
  #define GOTT_HASH_SET <hash_set>
#endif

#ifdef _MSC_VER
  #define GOTT_NSHASH stdext
  #define _DEFINE_DEPRECATED_HASH_CLASSES 0

  #include GOTT_HASH_MAP

  #define GOTT_HASH_BASE hash_compare
  #define GOTT_HASH_CLASS hash_compare
#endif

#ifndef _MSC_VER
  namespace GOTT_NSHASH {
    template<class> struct hash;
    #define GOTT_HASH_CLASS hash
    template<class> struct GOTT_HASH_BASE {};
  }
#endif

#endif
