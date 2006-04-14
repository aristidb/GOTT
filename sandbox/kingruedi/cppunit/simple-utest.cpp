#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cstring>

class String {
  std::size_t n;
  char * str;
public:
  String() : n(0), str(0x0) { }
  String(char const *s)
    : n(std::strlen(s)),
      str(new char[n])
  { }
  String(String const &s)
    : n(s.n),
      str(new char[n])
  {
    std::memcpy(str, s.str, n);
  }

  ~String() { delete[] str; }

  String &operator=(String const &s) {
    if(this != &s) {
      delete [] str;
      n = s.n;
      str = new char[n];
      std::memcpy(str, s.str, n);
    }
    return *this;
  }

  String &operator+=(String const &s) {
    char *tmp = new char[n+s.n];
    std::memcpy(tmp, str, n);
    delete [] str;
    str = tmp;
    std::memcpy(str+n, s.str, s.n);
    n += s.n;
    return *this;
  }

  friend bool operator==(String const &lhs, String const &rhs) {
    return lhs.n == rhs.n && std::memcmp(lhs.str, rhs.str, lhs.n);
  }
};

class StringTest 
  : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(StringTest);
   CPPUNIT_TEST(testCmp);
   CPPUNIT_TEST(testAdd); 
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp() {}
  void tearDown() {}

  void testCmp() {
    String a("abc");
    String b("abc");
    CPPUNIT_ASSERT(a == b);
    String c("def");
    CPPUNIT_ASSERT(!(a == c));
  }

  void testAdd() {
    String a("ab");
    String b("cd");
    a+=b;
    String c("abcd");
    CPPUNIT_ASSERT(a == c);
  }
};
CPPUNIT_TEST_SUITE_REGISTRATION(StringTest);
