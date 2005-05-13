#ifndef ASL_SOCKET_WIN32_SOCKET_HPP_INCLUDED
#define ASL_SOCKET_WIN32_SOCKET_HPP_INCLUDED

#include <asl/socket/SocketInfo.hpp>
#include <asl/socket/SocketException.hpp>
#include <asl/socket/Address.hpp>

#include <winsock2.h>
#include <windows.h>

namespace asl
{

class Socket
{
private:
  SocketInfo info;

  sockaddr_in getAddress(Address const& addr);

public:
  Socket(AddressType type, Protocol protocol);

  Socket(SocketInfo const& info);

  ~Socket();

  void connect(Address const& addr);

  std::size_t send(void const* data, std::size_t size, int flags=0);

  int recv(void* data, std::size_t size, int flags=0);

  void bind(Address const& addr);

  SocketInfo accept();

  void listen(int length);

  bool readable(int sec=0, int usec=0) const;

  bool writable(int sec=0, int usec=0) const;

  bool error(int sec=0, int usec=0) const;
};

}

#endif

