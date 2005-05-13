#include <csignal>
#include <iostream>
#include <unistd.h>

volatile bool running = true;

void finish(int) { running = false; }

int main() {
  signal(SIGINT, finish);
  signal(SIGTERM, finish);

  while (running) {
    system("scons -u -Q 2>&1 >/dev/null");
    sleep(2);
  }
}
