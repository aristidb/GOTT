#ifdef USE_SIGC
#  include <sigc++/sigc++.h>
#else
#  include <boost/signals.hpp>
#  include <boost/bind.hpp>
#endif

#include <iostream>
#include <boost/ptr_container/ptr_vector.hpp>
#include "bench.hpp"

struct foo
#ifdef USE_SIGC
  : public sigc::trackable
#endif
{
  void member() { }
};

void bar() { }

enum { TESTS=10000 };

int main() {
  foo obj;
#ifdef USE_SIGC
  boost::ptr_vector<sigc::signal<void> > vec;
  for(size_t i=0; i<TESTS; ++i) {
    sigc::signal<void> *sig_ptr=new sigc::signal<void>;
    sig_ptr->connect( sigc::ptr_fun(&bar) );
    vec.push_back(sig_ptr);

    sigc::signal<void> *mem_ptr=new sigc::signal<void>;
    mem_ptr->connect( sigc::mem_fun(obj, &foo::member) );
    vec.push_back(mem_ptr);
  }
#else
  boost::ptr_vector<boost::signal<void ()> > vec;
  for(size_t i=0; i<TESTS; ++i) {
    boost::signal<void ()> *sig_ptr=new boost::signal<void ()>;
    sig_ptr->connect(&bar);
    vec.push_back(sig_ptr);

    boost::signal<void ()> *mem_ptr=new boost::signal<void ()>;
    mem_ptr->connect( boost::bind(&foo::member, obj) );
    vec.push_back(mem_ptr);
  }
#endif
  std::cout << bench::total_mem() << '\n';
}
