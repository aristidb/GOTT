#ifdef USE_SIGC
#  include <sigc++/sigc++.h>
#else
#  include <boost/signals.hpp>
#  include <boost/bind.hpp>
#endif

#include <iostream>
#include "bench.hpp"

enum { TESTS=10000 };

void bar() { }

void function_ptr() {
  boost::uint64_t start=bench::rdtsc();
#ifdef USE_SIGC
  sigc::signal<void> sig;
  sig.connect( sigc::ptr_fun(&bar) );
#else
  boost::signal<void ()> sig;
  sig.connect(&bar);
#endif
  for(size_t n=0; n<TESTS; ++n) {
    sig();
  }
  boost::uint64_t ticks=bench::rdtsc()-start;
  std::cout << "Function Ptr: " << ticks << '\n';
}

struct foo
#ifdef USE_SIGC
  : public sigc::trackable
#endif
{
  void member() { }
};

void member_ptr() {
  foo obj;
  boost::uint64_t start=bench::rdtsc();
#ifdef USE_SIGC
  sigc::signal<void> sig;
  sig.connect( sigc::mem_fun(obj, &foo::member) );
#else
  boost::signal<void ()> sig;
  sig.connect( boost::bind(&foo::member, obj) );
#endif
  for(size_t n=0; n<TESTS; ++n) {
    sig();
  }
  boost::uint64_t ticks=bench::rdtsc()-start;
  std::cout << "Member Ptr: " << ticks << '\n';
}

int main() {
  function_ptr();
  member_ptr();
}
