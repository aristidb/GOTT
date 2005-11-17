#include <gott/tut/tut.h>
#include <gott/tut/tut_reporter.h>
#include <gott/tut/tut_html.h>
#include <iostream>

namespace tut
{
  test_runner_singleton runner;
}

//#define HTML_REPORT

int main(int argc,const char* argv[])
{
#ifdef HTML_REPORT
  tut::html_reporter visi;
#else
  tut::reporter visi;
#endif

  if( (argc == 2 && (
          std::string(argv[1]) == "help"
          || std::string(argv[1]) == "--help"
        )) 
      || argc > 3 )
  {
#define A << argv[0] << 
    std::cerr << "Usage: "A" [regression] | [list] | [ group] [test]\n";
    std::cerr << "         List all groups: "A" list\n";
    std::cerr << "         Run all tests: "A" regression\n";
    std::cerr << "         Run one group: "A" std::auto_ptr\n";
    std::cerr << "         Run one test: "A" std::auto_ptr 3\n";
#undef A
  }

  tut::runner.get().set_callback(&visi);

  try
  {
    if( argc == 1 || (argc == 2 && std::string(argv[1]) == "regression") )
    {
      std::cerr << "Testing everything...\n";
      tut::runner.get().run_tests();
    }
    else if( argc == 2 && std::string(argv[1]) == "list" )
    {
      std::cerr << "Registered test groups:" << std::endl;
      tut::groupnames gl = tut::runner.get().list_groups();
      tut::groupnames::const_iterator i = gl.begin();
      tut::groupnames::const_iterator e = gl.end();
      while( i != e )
      {
        std::cerr << "  " << *i << std::endl;
        ++i;
      }
    }
    else if( argc == 2 && std::string(argv[1]) != "regression" )
    {
      std::cerr << "Testing " << argv[1] << "...\n";
      tut::runner.get().run_tests(argv[1]);
    }
    else if( argc == 3 )
    {
      std::cerr << "Testing " << argv[1] << ", test<" << argv[2] << ">...\n";
      tut::runner.get().run_test(argv[1],::atoi(argv[2]));
    }
  }
  catch( const std::exception& ex )
  {
    std::cerr << "tut raised ex: " << ex.what() << std::endl;
  }

  return !visi.all_ok();
}
