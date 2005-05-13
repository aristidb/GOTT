#ifndef ASL_SOCKET_LINUX_PROTOCOL_HPP_INCLUDED
#define ASL_SOCKET_LINUX_PROTOCOL_HPP_INCLUDED

#include <sys/types.h>
#include <sys/socket.h>

namespace asl
{
enum Protocol
{
  TCP = SOCK_STREAM,
  UDP = SOCK_DGRAM
};
}

#endif

