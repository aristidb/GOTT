#ifndef asl_SOCKET_SOCKETSTREAM_HPP_INCLUDED
#define asl_SOCKET_SOCKETSTREAM_HPP_INCLUDED

#include "config.hpp"
#include "SocketBuffer.hpp"
#include "Socket.hpp"
#include "Address.hpp"

#include <iostream>
#include <string>

namespace asl
{
template<typename CharT, class TraitsT = std::char_traits<CharT> >
class basic_SocketStream : public std::basic_iostream<CharT, TraitsT>
{
public:
  typedef CharT char_type;
  typedef TraitsT traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

private:
  basic_SocketBuffer<CharT, TraitsT> buf;
public:
  basic_SocketStream(Socket& sock)
  : std::basic_iostream<CharT, TraitsT>(0), buf(sock)
  {
    this->init(&buf);
  }
};

template<typename CharT, class TraitsT = std::char_traits<CharT> >
class basic_ClientSocket : public basic_SocketStream<CharT, TraitsT>
{
public:
  typedef CharT char_type;
  typedef TraitsT traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

private:
  Socket sock;

public:
  basic_ClientSocket(Address const& addr)
  : basic_SocketStream<CharT, TraitsT>(sock),
    sock(addr.type(), TCP)
  {
    sock.connect(addr);
  }

  Socket& socket()
  {
    return sock;
  }

  Socket const& socket() const
  {
    return sock;
  }
};
template<typename CharT, class TraitsT = std::char_traits<CharT> >
class basic_AcceptSocket : public basic_SocketStream<CharT, TraitsT>
{
public:
  typedef CharT char_type;
  typedef TraitsT traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

private:
  Socket sock;

public:
  basic_AcceptSocket(SocketInfo const& info)
  : basic_SocketStream<CharT, TraitsT>(sock),
    sock(info)
  {}

  Socket& socket()
  {
    return sock;
  }

  Socket const& socket() const
  {
    return sock;
  }
};

class ServerSocket
{
private:
  Socket sock;
public:
  ServerSocket(Address const& addr, int listen=10)
  : sock(addr.type(), TCP)
  {
    sock.bind(addr);
    sock.listen(listen);
  }

  Socket& socket()
  {
    return sock;
  }

  Socket const& socket() const
  {
    return sock;
  }

  SocketInfo accept()
  {
    return sock.accept();
  }
};


typedef basic_ClientSocket<char> ClientSocket;
typedef basic_AcceptSocket<char> AcceptSocket;
}
#endif
