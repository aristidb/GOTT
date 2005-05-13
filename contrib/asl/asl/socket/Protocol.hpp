#ifndef ASL_SOCKET_PROTOCOL_HPP_INCLUDED
#define ASL_SOCKET_PROTOCOL_HPP_INCLUDED

#include <asl/socket/config.hpp>

#if ASL_OS == ASL_OS_WIN32
  #include <asl/socket/win32/Protocol.hpp>
#elif ASL_OS == ASL_OS_LINUX
  #include <asl/socket/linux/Protocol.hpp>
#else
  #error "unsupported OS"
#endif

#endif

