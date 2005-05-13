extern "C" {
#define _GCC_LIMITS_H_ // VERY strange bugfix
#include <fam.h>
}

#include <iostream>

char *appName = "autobuild";

int main() {
  FAMConnection fc;
  if (!FAMOpen(&fc)) {
    std::cerr << "Could not connect to FAM! Is it started?\n";
    return 1;
  }
  FAMClose(&fc);
}
