/*
* Copyright 2005 Vinzenz Feenstra - evilissimo@users.sourceforge.net
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/ 
#include <ebl/view/array.hpp>
#include <ebl/view/view.hpp>
#include <iostream>
#include <list>
#include <vector>

int main()
{
  int * anArray = new int[20](); 
  std::list<int> test_list(anArray,anArray+20);
  std::vector<int> test_vector(anArray,anArray+20);
  delete[] anArray;
  int num = 0;
  std::list<int>::iterator lbeg = test_list.begin(),
	                   lend = test_list.end();
  while(lbeg != lend)
  {
    *lbeg = num++;
    ++lbeg;
  }

  for(unsigned i = 0; i < 20; ++i)
      test_vector[i] = i;
  
  using namespace ebl::view;
 


  std::cout << getView<array_view>(test_list)[19] << std::endl; 
  std::cout << getView<array_view>(test_vector)[19] << std::endl;
  
  getView<array_view>(test_list)[10]   = 19;
  getView<array_view>(test_vector)[10] = 20; 
  
  
  std::cout << getView<array_view>(test_list)[10] << std::endl;
  std::cout << getView<array_view>(test_vector)[10] << std::endl;
  lbeg = test_list.begin();
  std::advance(lbeg,10); 
  std::cout << test_vector[10] << std::endl;

}
