#include <asl/socket/SocketException.hpp>

#include <cerrno>
#include <cstring>
#include <string>
#include <sstream>
#include <stdexcept>

namespace asl
{
private:
  static std::string build_error_string(int error, char const* msg, char const* file, std::size_t line, char const* func)
  {
    std::ostringstream stream;

    stream<<"SocketException: '"<<msg<<"'\nin File "<<file<<" on line "<<line<<"\nin Function "<<func<<"\nReason: ";

    if(error==0)
      stream<<"unknown error";
    else
      stream<<strerror(error);

    stream<<'\n';

    return stream.str();
  }

public:
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

