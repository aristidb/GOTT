#include <tut.h>
#include <tut_reporter.h>
#include <iostream>

namespace tut
{
  test_runner_singleton runner;
}

int main()
{
  tut::reporter reporter;
  tut::runner.get().set_callback(&reporter);

  try
  {
    tut::runner.get().run_tests();
  }
  catch( const std::exception& ex )
  {
    std::cerr << "tut raised ex: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}

