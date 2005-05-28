#ifndef ASL_SOCKET_LINUX_SOCKETEXCEPTION_HPP_INCLUDED
#define ASL_SOCKET_LINUX_SOCKETEXCEPTION_HPP_INCLUDED

#include <string>
#include <boost/current_function.hpp>
#include <stdexcept>
#include <cerrno>

namespace asl
{
class SocketException : public std::runtime_error
{
private:
  int error;
  std::string msg;

public:
  SocketException(int error, char const* message, char const* file, std::size_t line, char const* function);

  ~SocketException() throw();

  virtual char const* what() const throw();
};
}

#define ASL_SOCKET_EXCEPTION(msg) ::asl::SocketException(::errno, (msg), __FILE__, __LINE__, BOOST_CURRENT_FUNCTION)

#endif