
#include <asl/socket/config.hpp>

#if ASL_OS == ASL_OS_WIN32
  #include "win32/Address.cpp"
#elif ASL_OS == ASL_OS_LINUX
  #include "linux/Address.cpp"
#else
  #error "unsupported OS"
#endif

