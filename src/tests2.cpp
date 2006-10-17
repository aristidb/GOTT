// wegen ODR tests

#include <testsoon.hpp>

TEST() {
}

TEST() {
}

XTEST((generator, ::testsoon::int_generator<3>)) {
  equals(generator, 0);
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

struct startgenerator {
    int start;
    typedef int const& const_reference;
    ::testsoon::int_generator<10> rgen;
    typedef ::testsoon::int_generator<10>::iterator iterator;
    startgenerator(int i = 0) : start(i) {}
    iterator begin() { return iterator(start); }
    iterator end() { return rgen.end(); }
};

XTEST((gen, startgenerator)) {
    equals(generator, 3);
}

XTEST((gen, startgenerator) (genp, (4))) {
    equals(generator, 4);
}

XTEST((gf, 1)) {
  equals(typeid(group_fixture), typeid(group_fixture_t));
}

}}
