#include <testsoon.hpp>

TEST_REGISTRY

int main() {
  //testsoon::default_reporter rep;
  testsoon::xml_reporter rep;
  return !testsoon::tests().run(rep);
}
