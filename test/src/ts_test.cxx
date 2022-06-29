#include "ts_test.h"

int main() {
  auto a = Ts::relate_systems<Ts::System::A,  Ts::System::B1, Connector>();
  auto b = Ts::relate_systems<Ts::System::B1, Ts::System::A,  Connector>();
  auto c = Ts::relate_systems<Ts::System::B1, Ts::System::B2, Connector>();
  auto d = Ts::relate_systems<Ts::System::B2, Ts::System::B1, Connector>();
  auto e = Ts::relate_systems<Ts::System::C3, Ts::System::C2, Connector>();
  auto f = Ts::relate_systems<Ts::System::C3, Ts::System::C1, Connector>();
  auto g = Ts::relate_systems<Ts::System::C3, Ts::System::C3, Connector>();

  a.print_log();
  b.print_log();
  c.print_log();
  d.print_log();
  e.print_log();
  f.print_log();
  g.print_log();

}

