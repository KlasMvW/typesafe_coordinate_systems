#include "cs_test.h"

int main() {
  auto a = relateSystems<System::A, System::B1, Connector>();
  auto b = relateSystems<System::B1, System::A, Connector>();
  auto c = relateSystems<System::B1, System::B2, Connector>();
  auto d = relateSystems<System::B2, System::B1, Connector>();
  auto e = relateSystems<System::C3, System::C2, Connector>();
  auto f = relateSystems<System::C3, System::C1, Connector>();
  auto g = relateSystems<System::C3, System::C3, Connector>();

  a.print_log();
  b.print_log();
  c.print_log();
  d.print_log();
  e.print_log();
  f.print_log();
  g.print_log();

}

