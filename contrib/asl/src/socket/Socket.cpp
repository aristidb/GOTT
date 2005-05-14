
#include <asl/socket/config.hpp>

#if ASL_OS == ASL_OS_WIN32
  #include "win32/Socket.cpp"
#elif ASL_OS == ASL_OS_LINUX
  #include "linux/Socket.cpp"
#else
  #error "unsupported OS"
#endif

