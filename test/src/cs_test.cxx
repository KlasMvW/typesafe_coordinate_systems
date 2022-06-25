#include "cs_test.h"

int main() {
  auto a = Cs::relate_system<Cs::System::A,  Cs::System::B1, Connector>();
  auto b = Cs::relate_system<Cs::System::B1, Cs::System::A,  Connector>();
  auto c = Cs::relate_system<Cs::System::B1, Cs::System::B2, Connector>();
  auto d = Cs::relate_system<Cs::System::B2, Cs::System::B1, Connector>();
  auto e = Cs::relate_system<Cs::System::C3, Cs::System::C2, Connector>();
  auto f = Cs::relate_system<Cs::System::C3, Cs::System::C1, Connector>();
  auto g = Cs::relate_system<Cs::System::C3, Cs::System::C3, Connector>();

  a.print_log();
  b.print_log();
  c.print_log();
  d.print_log();
  e.print_log();
  f.print_log();
  g.print_log();

}

