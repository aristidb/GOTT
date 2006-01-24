// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)

#include "errno.hpp"

int main() {
  close(666);
  gott::throw_errno_exception();
}
