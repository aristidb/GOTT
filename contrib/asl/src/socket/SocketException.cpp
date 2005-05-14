
#include <asl/socket/config.hpp>

#if ASL_OS == ASL_OS_WIN32
  #include "win32/SocketException.cpp"
#elif ASL_OS == ASL_OS_LINUX
  #include "linux/SocketException.cpp"
#else
  #error "unsupported OS"
#endif

