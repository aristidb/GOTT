#include <gott/util/tut/tut.h>
#include <gott/util/tut/tut_reporter.h>
#include <iostream>

namespace tut
{
  test_runner_singleton runner;
}

int main(int argc,const char* argv[])
{
  tut::reporter visi;

  if( (argc == 2 && (
          std::string(argv[1]) == "help"
          || std::string(argv[1]) == "--help"
        )) 
      || argc > 3 )
  {
#define A << argv[0] << 
    std::cout << "Usage: "A" [regression] | [list] | [ group] [test]\n";
    std::cout << "         List all groups: "A" list\n";
    std::cout << "         Run all tests: "A" regression\n";
    std::cout << "         Run one group: "A" std::auto_ptr\n";
    std::cout << "         Run one test: "A" std::auto_ptr 3\n";
#undef A
  }

  tut::runner.get().set_callback(&visi);

  try
  {
    if( argc == 1 || (argc == 2 && std::string(argv[1]) == "regression") )
    {
      std::cout << "Testing everything...\n";
      tut::runner.get().run_tests();
    }
    else if( argc == 2 && std::string(argv[1]) == "list" )
    {
      std::cout << "Registered test groups:" << std::endl;
      tut::groupnames gl = tut::runner.get().list_groups();
      tut::groupnames::const_iterator i = gl.begin();
      tut::groupnames::const_iterator e = gl.end();
      while( i != e )
      {
        std::cout << "  " << *i << std::endl;
        ++i;
      }
    }
    else if( argc == 2 && std::string(argv[1]) != "regression" )
    {
      std::cout << "Testing " << argv[1] << "...\n";
      tut::runner.get().run_tests(argv[1]);
    }
    else if( argc == 3 )
    {
      std::cout << "Testing " << argv[1] << ", test<" << argv[2] << ">...\n";
      tut::runner.get().run_test(argv[1],::atoi(argv[2]));
    }
  }
  catch( const std::exception& ex )
  {
    std::cerr << "tut raised ex: " << ex.what() << std::endl;
  }

  return 0;
}
