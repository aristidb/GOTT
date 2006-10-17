// wegen ODR tests

#include <testsoon.hpp>


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

using testsoon::range_generator;

TEST_GROUP(_2_4) {

XTEST((gen, (range_generator<int>)(2)(4))) {
  equals(generator, 3);
}

}

TEST_GROUP(_4_6) {

XTEST((gen, (range_generator<int>)(4)(6))) {
  equals(generator, 4);
}

}

XTEST((gf, 1)) {
  equals(typeid(group_fixture), typeid(group_fixture_t));
}

}}
