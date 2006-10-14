// wegen ODR tests

#include <testsoon.hpp>

TEST() {
}

TEST_GROUP(yessir) { TEST_GROUP(nested) {

XTEST((name, "dummy") (name, "foo")) {
}

XTEST((fixture, muh)) {
}

}}
