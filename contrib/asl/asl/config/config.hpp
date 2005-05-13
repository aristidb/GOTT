#ifndef ASL_CONFIG_CONFIG_HPP_INCLUDED
#define ASL_CONFIG_CONFIG_HPP_INCLUDED

#define ASL_OS_LINUX 0x1
#define ASL_OS_WIN32 0x2

#if defined(WIN32) || defined(__WIN32__)
  #define ASL_OS ASL_OS_WIN32
#elif defined(linux) || defined(__linux__)
  #define ASL_OS ASL_OS_LINUX
#else
  #error "unknown/unsupported operating system"
#endif

//TODO: add #defines
//TODO: compiler detection
#endif

