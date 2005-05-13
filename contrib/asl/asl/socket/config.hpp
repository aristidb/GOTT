#ifndef ASL_SOCKET_CONFIG_HPP_INCLUDED
#define ASL_SOCKET_CONFIG_HPP_INCLUDED

#include <asl/config/config.hpp>

#if ASL_OS == ASL_OS_WIN32
#include <winsock2.h>

namespace asl
{
namespace hidden
{
inline void ShutdownSockets()
{
  WSACleanup();
}
}
inline void InitSockets()
{
  static bool initialized=false;
  if(!initialized)
  {
    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2,0),&wsadata)) //winsock 2.0
      throw std::runtime_error("WSAStartup failed");
    std::atexit(hidden::ShutdownSockets);
  }
  initialized=true;
}
}

#elif ASL_OS == ASL_OS_LINUX
namespace asl
{
inline void InitSockets()
{}
}
#endif

#endif

