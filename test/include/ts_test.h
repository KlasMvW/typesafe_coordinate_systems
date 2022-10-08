#pragma once

#include "ts/typesafe_system.h"
#include <vector>
#include <string>
namespace ts {
// Define geometry for testing. A0 and D0 are the root nodes of separate trees

/*
 *                   Root
 *         A0                    D0
 *      A1    B1
 *    A2  B2    C2
 *            A3  B3
 *
 */

struct A0 : System<A0, Root> {};
struct A1 : System<A1, A0> {};
struct B1 : System<B1, A0> {};
struct A2 : System<A2, A1> {};
struct B2 : System<B2, A1> {};
struct C2 : System<C2, B1> {};
struct A3 : System<A3, C2> {};
struct B3 : System<A3, C2> {};

struct D0 : System<D0, Root> {};

template <typename T, typename From, typename To>
struct DummyMatrix {
  std::vector<std::string> trace;
};

// Define dummy structs for testing

struct A {};
struct B {};
struct C {};
struct D {};
struct E {};

// Define transformation functions that must be availabe for some functions in TSCoordinateSystem.h to work
namespace transform {
void toChild(const A&, const B) {}
void toChild(const A&, B&, const C) {}
void toChild(A&, B, C&, const D) {}
void toChild(A&, B, const C&, const D, E) {}

template <typename T, typename From>
void toChild(const DummyMatrix<T, From, A0>& from, DummyMatrix<T, From, B1>& to, const A&, const B&) {
  to.trace = from.trace;
  to.trace.push_back("A0_to_B1");
}

template <typename T, typename From>
void toChild(const DummyMatrix<T, From, B1>& from, DummyMatrix<T, From, C2>& to, const A&, const B&) {
  to.trace = from.trace;
  to.trace.push_back("B1_to_C2");
}

template <typename T, typename From>
void toChild(const DummyMatrix<T, From, C2>& from, DummyMatrix<T, From, B3>& to, const A&, const B&) {
  to.trace = from.trace;
  to.trace.push_back("C2_to_B3");
}

template <typename T, typename From>
void toChild(const DummyMatrix<T, From, C2>& from, DummyMatrix<T, From, A3>& to, const A&, const B&) {
  to.trace = from.trace;
  to.trace.push_back("C2_to_A3");
}

template <typename T, typename From>
DummyMatrix<T, From, A0> toParent(const DummyMatrix<T, From, A1>& mIn, const A&, const B&) {
  DummyMatrix<T, From, A0> mOut;
  mOut.trace = mIn.trace;
  mOut.trace.push_back("A1_to_A0");
  return mOut;
}

template <typename T, typename From>
DummyMatrix<T, From, A1> toParent(const DummyMatrix<T, From, A2>& mIn, const A&, const B&) {
  DummyMatrix<T, From, A1> mOut;
  mOut.trace = mIn.trace;
  mOut.trace.push_back("A2_to_A1");
  return mOut;
}

} // namespace transform

} // namespace ts