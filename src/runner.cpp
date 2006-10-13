#include "test.hpp"

TEST_REGISTRY

int main() {
  testsoon::default_reporter rep;
  testsoon::tests().run(rep);
}
