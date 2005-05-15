#include <iostream>
#include <vector>
#include <list>
#include <string>
#include "../view.hpp"
#include "../string_view.hpp"

void test_w()
{
  std::list< int > vec;
  
  for(int i = 0; i < 50; ++i)
	vec.push_back(i);
  
  typedef ebl::view::string_view< std::wstring , std::list< int > > WStrView;
  WStrView Wt( vec , L'|' );
  std::wcout << Wt.get() << std::endl;
}
void test()
{
  typedef ebl::view::string_view< std::string , std::list< int > >  StrView;
  std::list< int > vec;
  for(int i = 0; i < 50; ++i)
	vec.push_back(i);
  StrView   t( vec ,  '|' );
  std::cout  <<  t.get() << std::endl;
}

int main()
{
test();
test_w();

}
