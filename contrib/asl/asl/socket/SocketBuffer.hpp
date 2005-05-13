#ifndef asl_SOCKET_SOCKETBUFFER_HPP_INCLUDED
#define asl_SOCKET_SOCKETBUFFER_HPP_INCLUDED

#include "config.hpp"
#include "Socket.hpp"

#include <streambuf>
#include <string>

namespace asl
{

template<typename CharT, class TraitsT = std::char_traits<CharT> >
class basic_SocketBuffer : public std::basic_streambuf<CharT, TraitsT>
{
public:
  typedef CharT char_type;
  typedef TraitsT traits_type;
  typedef typename traits_type::int_type int_type;
  typedef typename traits_type::pos_type pos_type;
  typedef typename traits_type::off_type off_type;

protected:
  Socket& sock;
  static const int pbSize = 4;
  static const int bufSize = 1024;
  CharT buffer[bufSize+pbSize];

public:
  basic_SocketBuffer(Socket& sock)
  : sock(sock)
  {

    this->setg(buffer+pbSize,
               buffer+pbSize,
               buffer+pbSize
    );
  }

protected:
  virtual int_type overflow(int_type c)
  {
    if(!traits_type::eq_int_type(c, traits_type::eof()))
    {
      CharT z=c;
      if(sock.send(&z, sizeof(CharT))!=sizeof(CharT))
        return traits_type::eof();
    }
    return traits_type::not_eof(c);
  }

  virtual std::streamsize xsputn(CharT const* s, std::streamsize n)
  {
    return sock.send(s, n*sizeof(CharT));
  }

  virtual int_type underflow()
  {
    if(this->gptr()<this->egptr())
      return traits_type::to_int_type(*this->gptr());

    int numPutback = this->gptr() - this->eback();
    if(numPutback > pbSize)
    {
      numPutback = pbSize;
    }

    std::memmove(buffer+(pbSize-numPutback), this->gptr()-numPutback, numPutback*sizeof(CharT));

    int num = sock.recv(buffer+pbSize, bufSize*sizeof(CharT));
    if(num <= 0)
    {
      return traits_type::eof();
    }

    this->setg(buffer+(pbSize-numPutback),
               buffer+pbSize,
               buffer+pbSize+num
    );

    return traits_type::to_int_type(*this->gptr());
  }
};
}

#endif
