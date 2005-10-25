#ifndef BENCH_HPP_INCLUDED
#define BENCH_HPP_INCLUDED

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <boost/cstdint.hpp>

//This will be extend
//Just a simple hack

namespace bench {
  namespace misc {
    template<typename T>
    std::string Ttos(T i) {
      std::string ret;
      do {
	int n=i%10;
	ret += n+'0';
      } while( i/=10 );
      std::reverse(ret.begin(), ret.end());
      return ret;
    }

    template<typename T>
    T stoT(std::string s) {
      std::reverse(s.begin(), s.end());
      T ret=0;
      T k=1;
      for(std::string::const_iterator i=s.begin();
	  i!=s.end();
	  ++i)
	{
	  ret+=(*i-'0')*k;
	  k*=10;
	}
      return ret;
    }
  }

  inline std::size_t total_mem(pid_t pid=getpid()) {
    std::string str="/proc/";
    str+=misc::Ttos(pid)+"/statm";
    int fd=open(str.c_str(), O_RDONLY);
    if(fd == -2)
      throw 0;

    enum { BUF_SIZE=255 };
    char buf[BUF_SIZE];
    ssize_t n=read(fd, buf, BUF_SIZE);
    if(n == -1)
      throw 0;
    for(ssize_t i=0; i<n; ++i)
      if(buf[i] < '0' || buf[i] > '9')
	buf[i]=0;

    return misc::stoT<std::size_t>(buf);
  }

  inline ::boost::uint64_t rdtsc() {
    ::boost::uint32_t eax, edx;
    __asm__ __volatile__ ("rdtsc; mov %%eax, %0; mov %%edx, %1" : "=r" (eax)
			  , "=r"(edx) : : "%eax", "%edx");
    return (::boost::uint64_t(edx) << 32) | ::boost::uint64_t(eax);
  }
}

#endif
