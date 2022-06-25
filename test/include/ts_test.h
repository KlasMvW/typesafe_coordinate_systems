#pragma once

#include "ts/typesafe_coordinate_systems.h"
#include "connector.h"

#include <vector>
#include <string>

//                A
//              /  \
//             B1  B2
//           /  \   \
//         C1   C2   C3
//
//
//
//
//

namespace Ts {
namespace System
{
  struct A : System<A, Root>
  {
  };

  struct B1 : System<B1, A>
  {
  };

  struct B2 : System<B2, A>
  {
  };

  struct C1 : System<C1, B1>
  {
  };

  struct C2 : System<C2, B1>
  {
  };

  struct C3 : System<C3, B2>
  {
  };
}

namespace Transform
{
 // A <-> B1
  template<typename From>
  Connector<From, System::A> to_parent(const Connector<From, System::B1>& from)
  {
    Connector<System::B1, System::A> B1_to_A;
    B1_to_A.log_message("B1 to A");

    Connector<From, System::A> sr = B1_to_A * from;
    return sr;
  }

  template<typename From>
  void to_child(const Connector<From, System::A>& from, Connector<From, System::B1>& to)
  {
    Connector<System::A, System::B1> A_to_B1;
    A_to_B1.log_message("A to B1");

    to = A_to_B1 * from;
  }

 // A <-> B2
  template<typename From>
  Connector<From, System::A> to_parent(const Connector<From, System::B2>& from)
  {
    Connector<System::B2, System::A> B2_to_A;
    B2_to_A.log_message("B2 to A");

    Connector<From, System::A> sr = B2_to_A * from;
    return sr;
  }

  template<typename From>
  void to_child(const Connector<From, System::A>& from, Connector<From, System::B2>& to)
  {
    Connector<System::A, System::B2> A_to_B2;
    A_to_B2.log_message("A to B2");

    to = A_to_B2 * from;
  }

  // B2 <-> C3
  template<typename From>
  Connector<From, System::B2> to_parent(const Connector<From, System::C3>& from)
  {
    Connector<System::C3, System::B2> C3_to_B2;
    C3_to_B2.log_message("C3 to B2");

    Connector<From, System::B2> sr = C3_to_B2 * from;
    return sr;
  }

  template<typename From>
  void to_child(const Connector<From, System::B2>& from, Connector<From, System::C3>& to)
  {
    Connector<System::B2, System::C3> B2_to_C3;
    B2_to_C3.log_message("B2 to C3");

    to = A_to_B2 * from;
  }

  // B1 <-> C1
  template<typename From>
  Connector<From, System::B1> to_parent(const Connector<From, System::C1>& from)
  {
    Connector<System::C1, System::B1> C1_to_B1;
    C1_to_B1.log_message("C1 to B1");

    Connector<From, System::B1> sr = C1_to_B1 * from;
    return sr;
  }

  template<typename From>
  void to_child(const Connector<From, System::B1>& from, Connector<From, System::C1>& to)
  {
    Connector<System::B1, System::C1> B1_to_C1;
    B1_to_C1.log_message("B1 to C1");

    to = B1_to_C1 * from;
  }

  // B1 <-> C2
  template<typename From>
  Connector<From, System::B1> to_parent(const Connector<From, System::C2>& from)
  {
    Connector<System::C2, System::B1> C2_to_B1;
    C2_to_B1.log_message("C2 to B1");

    Connector<From, System::B1> sr = C2_to_B1 * from;
    return sr;
  }

  template<typename From>
  void to_child(const Connector<From, System::B1>& from, Connector<From, System::C2>& to)
  {
    Connector<System::B1, System::C2> B1_to_C2;
    B1_to_C2.log_message("B1 to C2");

    to = B1_to_C2 * from;
  }
} // namespace Transform
} // namespace Cs