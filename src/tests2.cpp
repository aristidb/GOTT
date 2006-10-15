// wegen ODR tests

#include <testsoon.hpp>

TEST() {
}

TEST_GROUP(yessir) { TEST_GROUP(nested) {

XTEST((name, "dummy") (name, "foo")) {
  //(void)fixture; - does not compile
}

struct dummy_fixture {
};

XTEST((fixture, dummy_fixture)) {
  equals(typeid(fixture), typeid(dummy_fixture));
}

FTEST(, dummy_fixture) {
  equals(typeid(fixture), typeid(dummy_fixture));
}

}}
