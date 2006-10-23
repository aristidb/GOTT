#include <cxxtest/TestSuite.h>

class TestFail : public CxxTest::TestSuite
{
public:
  void testFail() {
    TS_ASSERT(false);
  }
};
