#include "test.hpp"

TEST_REGISTRY

int main() {
  test_soon::default_reporter<> rep;
  test_soon::tests().run(rep);
}
