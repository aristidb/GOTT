#ifndef ASL_SOCKET_LINUX_SOCKETINFO_HPP_INCLUDED
#define ASL_SOCKET_LINUX_SOCKETINFO_HPP_INCLUDED

#include <netinet/in.h>

namespace asl
{
typedef int socket_t;

struct SocketInfo
{
  socket_t sock;
  in_addr address;
  int port;
};
}

#endif

