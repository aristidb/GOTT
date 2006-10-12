#include "test.hpp"

TEST_REGISTRY;

int main() {
  default_reporter<> rep;
  tests().run(rep);
}
