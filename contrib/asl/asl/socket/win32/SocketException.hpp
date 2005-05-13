#ifndef ASL_SOCKET_WIN32_SOCKETEXCEPTION_HPP_INCLUDED
#define ASL_SOCKET_WIN32_SOCKETEXCEPTION_HPP_INCLUDED

#include <string>
#include <boost/current_function.hpp>
#include <stdexcept>

#include <winsock2.h>

namespace asl
{
class SocketException : public std::runtime_error
{
private:
  std::string msg;

  static std::string build_error_string(int error, char const* msg, char const* file, std::size_t line, char const* func);

public:
  SocketException(int error, char const* message, char const* file, std::size_t line, char const* function);

  ~SocketException() throw();

  virtual char const* what() const throw();
};
}
#define ASL_SOCKET_EXCEPTION(msg) ::asl::SocketException(::WSAGetLastError(), (msg), __FILE__, __LINE__, BOOST_CURRENT_FUNCTION)

#endif
