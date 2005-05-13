#ifndef asl_SOCKET_SOCKETEXCEPTION_HPP_INCLUDED
#define asl_SOCKET_SOCKETEXCEPTION_HPP_INCLUDED

#include <asl/socket/config.hpp>

#if ASL_OS == ASL_OS_WIN32
  #include <asl/socket/win32/SocketException.hpp>
#elif ASL_OS == ASL_OS_LINUX
  #include <asl/socket/linux/SocketException.hpp>
#else
  #error "unsupported OS"
#endif

#endif

