#ifndef ASL_SOCKET_ADDRESS_HPP_INCLUDED
#define ASL_SOCKET_ADDRESS_HPP_INCLUDED

#include <asl/socket/config.hpp>

#if ASL_OS == ASL_OS_WIN32
  #include <asl/socket/win32/Address.hpp>
#elif ASL_OS == ASL_OS_LINUX
  #include <asl/socket/linux/Address.hpp>
#else
  #error "unsupported OS"
#endif


#endif

