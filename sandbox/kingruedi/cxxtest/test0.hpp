#include "string.hpp"

#include <cxxtest/TestSuite.h>

class string_test : public CxxTest::TestSuite
{
public:
  void testCmp() {
    String a = "abc";
    String b = "abc";

    TS_ASSERT( a == b );

    String c = "def";
    TS_ASSERT( !(a == c) );
  }

  void testAdd() {
    String a = "ab";
    String b = "cd";
    String c = "abcd";
    a += b;
    TS_ASSERT(a == c);
  }
};
