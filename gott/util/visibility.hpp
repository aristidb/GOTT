#ifndef GOTT_UTIL_VISIBILITY_HPP
#define GOTT_UTIL_VISIBILITY_HPP

#ifdef _MSC_VER
  #ifdef BUILDING_DLL
    #define GOTT_EXPORT __declspec(dllexport)
  #else
    #define GOTT_EXPORT __declspec(dllimport)
  #endif
  #define GOTT_LOCAL
#else
  #ifdef HAVE_VISIBILITY
    #define GOTT_EXPORT __attribute__ ((visibility("default")))
    #define GOTT_LOCAL __attribute__ ((visibility("hidden")))
  #else
    #define GOTT_EXPORT
    #define GOTT_LOCAL
  #endif
#endif

#endif
