#include <testsoon.hpp>

TEST_REGISTRY

int main() {
  testsoon::default_reporter rep;
  return !testsoon::tests().run(rep);
}
