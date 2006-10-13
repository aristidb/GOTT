#include <testsoon.hpp>
#include <exception>

int get_no() { return 7; }
void foo() {}
struct exception_type {};

TEST(name) {
  int a = get_no();
  equals(a, 7);
  throws(foo(), std::exception, "value of .what()");
}

TEST_GROUP(yessir) { TEST_GROUP(nested) {

TEST() {
  nothrows(throw 0, ...);
}

TEST() {
  equals(1,2);
}

} //nested
} //yessir
