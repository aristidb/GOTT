#include "../container_traits.hpp"

#include <typeinfo>
#include <iostream>
#include <set>
#include <vector>
#include <stack>
#include <deque>
#include <list>
#include <map>

template < class T >
  struct Test
  { 
    typedef ebl::util::type_traits::container_traits<T> ContainerTraits;
    typedef typename ContainerTraits::reference       reference;
    typedef typename ContainerTraits::const_reference const_reference;
    typedef typename ContainerTraits::iterator        iterator;
    typedef typename ContainerTraits::const_iterator  const_iterator;
    typedef typename ContainerTraits::allocator_type  allocator_type;
    typedef typename ContainerTraits::value_type      value_type;
  };

struct _INT_
{
  bool operator < ( _INT_ const & lhs ) const{}
};

Test< std::vector<_INT_> >  Test1;
Test< std::set<_INT_> >     Test2;
Test< std::list<_INT_> >    Test3;
Test< std::stack<_INT_> >   Test4;
Test< std::deque<_INT_> >   Test5;
Test< std::map<_INT_,_INT_> > Test6;


int main()
{
  std::cout << typeid(Test1).name() << std::endl;
  std::cout << typeid(Test2).name() << std::endl;
  std::cout << typeid(Test3).name() << std::endl;
  std::cout << typeid(Test4).name() << std::endl;
  std::cout << typeid(Test5).name() << std::endl;
  std::cout << typeid(Test6).name() << std::endl;
}
