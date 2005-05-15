#include <ebl/view/array.hpp>
#include <ebl/view/view.hpp>

#include <vector>
#include <list>

#include <tut.h>
#include <set>

namespace tut
{
  struct array_view_data
  { 
    std::vector<int> vec;
    std::list<int>   list;
    std::set<int>    set;
    
    array_view_data()
    {
      int tmp[40]; 
      vec = std::vector<int>(tmp,tmp+40);
      list = std::list<int>(tmp,tmp+40);
    }
  };

  typedef test_group<array_view_data> tg;
  typedef tg::object object;
  tg array_view_group("array_view");

  /**
   * checking std::list<int>
   */
  template<>
  template<>
  void object::test<1>()
  {
    using ebl::view::getView;
    using ebl::view::array_view;
    // Filling std::list via operator[]
    for(int i = 0; i < 40; ++i)
	  getView<array_view>(list)[i] = i;

    std::list<int>::iterator beg = list.begin(),
	                     end = list.end();
    int comp = 0;
    while(beg != end)
    {
      ensure_equals("Filled std::list<int> via operator[]",*beg,comp);
      ++comp; ++beg;
    }	     
  }
  /**
   * checking std::vector<int> [ Has specialisation ]
   */
  template<>
  template<>
  void object::test<2>()
  {
    using ebl::view::getView;
    using ebl::view::array_view;
    
    for(int i = 0; i < 40; ++i)
       getView<array_view>(vec)[i] = i;

    for(int i = 0; i < 40; ++i)
	ensure_equals("Filled std::vector<int> via operator[] from array_view",
		      vec[i], i); // ensures that vec[i] equals i 
  }

  /**
   * checking std::set<int> 
   */
  int const & get ( int const & t ){ return t; }
  template<>
  template<>
  void object::test<3>()
  {
  
    using ebl::view::getView;
    using ebl::view::array_view;
    
    for(int i = 0; i < 40; ++i)
        set.insert(i);	
    array_view< std::set<int> > t = getView<array_view>(set);
    
    for(int i = 0; i < 40; ++i)
	ensure_equals("Reading std::set<int> via operator[] from array_view",
		      i , get(t[i]) );
  }
  
}


