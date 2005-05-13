#ifndef ASL_SOCKET_WIN32_PROTOCOL_HPP_INCLUDED
#define ASL_SOCKET_WIN32_PROTOCOL_HPP_INCLUDED

#include <winsock2.h>

namespace asl
{
enum Protocol
{
  TCP = SOCK_STREAM,
  UDP = SOCK_DGRAM
};
}

#endif

