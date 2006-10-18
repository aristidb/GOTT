#include <testsoon.hpp>
#include <exception>

int get_no() { return 7; }
void foo() {}
struct exception_type {};

TEST(name) {
  int a = get_no();
  Equals(a, 7);
  Throws(foo(), std::exception, "value of .what()");
}

TEST_GROUP(yessir) { TEST_GROUP(nested) {

TEST() {
  Nothrows(throw 0, ...);
}

TEST() {
  Equals(1,2);
}

} //nested
} //yessir
