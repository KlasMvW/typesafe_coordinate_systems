#include "cs_test.h"

int main() {
  auto a = relate_system<System::A, System::B1, Connector>();
  auto b = relate_system<System::B1, System::A, Connector>();
  auto c = relate_system<System::B1, System::B2, Connector>();
  auto d = relate_system<System::B2, System::B1, Connector>();
  auto e = relate_system<System::C3, System::C2, Connector>();
  auto f = relate_system<System::C3, System::C1, Connector>();
  auto g = relate_system<System::C3, System::C3, Connector>();

  a.print_log();
  b.print_log();
  c.print_log();
  d.print_log();
  e.print_log();
  f.print_log();
  g.print_log();

}

