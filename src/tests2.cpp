// wegen ODR tests

#include <testsoon.hpp>

TEST() {
}

TEST() {
}

TEST() {
}

TEST_GROUP(yessir) {

TEST() {
} 

TEST_GROUP(nested) {

TEST() {
}

XTEST() {
}

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

struct group_fixture_t {};

XTEST((gf, 1)) {
  equals(typeid(group_fixture), typeid(group_fixture_t));
}

}}
