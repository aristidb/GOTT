#ifndef ASL_SOCKET_WIN32_SOCKETINFO_HPP_INCLUDED
#define ASL_SOCKET_WIN32_SOCKETINFO_HPP_INCLUDED

#include <winsock2.h>

namespace asl
{
typedef SOCKET socket_t;

struct SocketInfo
{
  socket_t sock;
  in_addr address;
  int port;
};
}

#endif

