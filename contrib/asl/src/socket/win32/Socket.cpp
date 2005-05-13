#include <asl/socket/Socket.hpp>

#include <asl/socket/SocketInfo.hpp>
#include <asl/socket/SocketException.hpp>
#include <asl/socket/Address.hpp>

#include <winsock2.h>
#include <windows.h>

namespace asl
{

sockaddr_in Socket::getAddress(Address const& addr)
{
  sockaddr_in address;
  address.sin_family=addr.type();
  address.sin_port=addr.port();
  address.sin_addr=*(in_addr*)addr.host();

  info.port=address.sin_port;
  info.address=address.sin_addr;

  return address;
}

Socket::Socket(AddressType type, Protocol protocol)
{
  info.sock=::socket(type, protocol, 0);
  if(info.sock==-1)
    throw ASL_SOCKET_EXCEPTION("socket failed");
}

Socket::Socket(SocketInfo const& info)
: info(info)
{}

Socket::~Socket()
{
  closesocket(info.sock);
}

void Socket::connect(Address const& addr)
{
  sockaddr_in address=getAddress(addr);
  int res=::connect(info.sock, (sockaddr*)&address, sizeof(address));
  if(res)
    throw ASL_SOCKET_EXCEPTION("connect failed");
}

std::size_t Socket::send(void const* data, std::size_t size, int flags)
{
  int res=::send(info.sock, (char*)data, size, flags);
  if(res<0)
    throw ASL_SOCKET_EXCEPTION("send failed");

  return res;
}

int Socket::recv(void* data, std::size_t size, int flags)
{
  return ::recv(info.sock, (char*)data, size, flags);
}

void Socket::bind(Address const& addr)
{
  sockaddr_in address=getAddress(addr);
  int res=::bind(info.sock, (sockaddr*)&address, sizeof(address));
  if(res)
    throw ASL_SOCKET_EXCEPTION("bind failed");
}

SocketInfo Socket::accept()
{
  sockaddr_in address;
  SocketInfo newInfo;
  int size=sizeof(address);
  newInfo.sock = ::accept(info.sock, (sockaddr*)&address, &size);
  if(newInfo.sock==-1)
    throw ASL_SOCKET_EXCEPTION("accept failed");

  newInfo.port=address.sin_port;
  newInfo.address=address.sin_addr;

  return newInfo;
}

void Socket::listen(int length)
{
  int res=::listen(info.sock, length);
  if(res)
    throw ASL_SOCKET_EXCEPTION("listen failed");
}

bool Socket::readable(int sec, int usec) const
{
  struct timeval time;
  time.tv_sec  = sec;
  time.tv_usec = usec;

  fd_set fd;
  FD_ZERO(&fd);
  FD_SET(info.sock, &fd);

  if(::select(info.sock+1, &fd, 0, 0, &time)==-1)
    throw ASL_SOCKET_EXCEPTION("select failed");

  return FD_ISSET(info.sock, &fd);
}

bool Socket::writable(int sec, int usec) const
{
  struct timeval time;
  time.tv_sec  = sec;
  time.tv_usec = usec;

  fd_set fd;
  FD_ZERO(&fd);
  FD_SET(info.sock, &fd);

  if(::select(info.sock+1, 0, &fd, 0, &time)==-1)
    throw ASL_SOCKET_EXCEPTION("select failed");

  return FD_ISSET(info.sock, &fd);
}

bool Socket::error(int sec, int usec) const
{
  struct timeval time;
  time.tv_sec  = sec;
  time.tv_usec = usec;

  fd_set fd;
  FD_ZERO(&fd);
  FD_SET(info.sock, &fd);

  if(::select(info.sock+1, 0, 0, &fd, &time)==-1)
    throw ASL_SOCKET_EXCEPTION("select failed");

  return FD_ISSET(info.sock, &fd);
}
}

