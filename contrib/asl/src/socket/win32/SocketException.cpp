#include <asl/socket/SocketException.hpp>

#include <windows.h>

#include <string>
#include <sstream>
#include <boost/current_function.hpp>
#include <stdexcept>


namespace asl
{
std::string SocketException::build_error_string(int error, char const* msg, char const* file, std::size_t line, char const* func)
{
  std::ostringstream stream;

  stream<<"SocketException: '"<<msg<<"'\nin File "<<file<<" on line "<<line<<"\nin Function "<<func<<"\nReason: ";

  LPTSTR MsgBuf;
  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    error,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &MsgBuf,
    0,
    NULL
  );
  stream<<MsgBuf;
  LocalFree(MsgBuf);

  stream<<'\n';

  return stream.str();
}

SocketException::SocketException(int error, char const* message, char const* file, std::size_t line, char const* function)
: std::runtime_error(message),
  msg(build_error_string(error, message, file, line, function))
{}

SocketException::~SocketException() throw()
{}

char const* SocketException::what() const throw()
{
  return msg.c_str();
}

}

