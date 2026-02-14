#include "ts_test.h"
#include "test_framework.h"
#include "ts/typesafe_coordinate_systems.h"

using namespace ts;

int main() {

Test<"testFindFunction">([]<typename T>([[maybe_unused]]T& t) {

  if constexpr (!find_function::to_child<A, B>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<A&, B>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<const A&, B&, const C>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<const A&, const B>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<A&, B, C&, const D>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<A&, B, const C&, const D, E>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<A, B&>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<A&, B&>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<const A&, B&>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<A&, const B&>) {
    t.assert_true(false, __LINE__);
  }

  //

  if constexpr (find_function::to_child<A, C>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (find_function::to_child<const A&, C>) {
    t.assert_true(false, __LINE__);
  }

  ///

  // These ARE callable: non-const lvalue args bind to non-const ref params
  if constexpr (!find_function::to_child<A, B, C>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (find_function::to_child<A, const B, C>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<A, B, C&>) {
    t.assert_true(false, __LINE__);
  }

  ///

  if constexpr (!find_function::to_child<A, B, C, D>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::to_child<A, B, C&, D>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (find_function::to_child<const A, B, C&, const D>) {
    t.assert_true(false, __LINE__);
  }

  ///

  if constexpr (!find_function::to_child<A, B, C, D, E>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (find_function::to_child<A, B, D, E, C>) {
    t.assert_true(false, __LINE__);
  }
});

// Test concept detection with DummyMatrix types — mirrors actual usage in down()
// Defined to_child pairs: A0->B1, B1->C2, C2->B3, C2->A3
Test<"testFindFunctionTemplate">([]<typename T>([[maybe_unused]]T& t) {
  using M = DummyMatrix<float, A0, A0>; // arbitrary From for type deduction

  // Existing direct parent-child pairs should be found
  if constexpr (!find_function::to_child<const DummyMatrix<float, A0, A0>&, DummyMatrix<float, A0, B1>&, const A&, const B&>) {
    t.assert_true(false, __LINE__);
  }
  if constexpr (!find_function::to_child<const DummyMatrix<float, A0, B1>&, DummyMatrix<float, A0, C2>&, const A&, const B&>) {
    t.assert_true(false, __LINE__);
  }
  if constexpr (!find_function::to_child<const DummyMatrix<float, A0, C2>&, DummyMatrix<float, A0, B3>&, const A&, const B&>) {
    t.assert_true(false, __LINE__);
  }
  if constexpr (!find_function::to_child<const DummyMatrix<float, A0, C2>&, DummyMatrix<float, A0, A3>&, const A&, const B&>) {
    t.assert_true(false, __LINE__);
  }

  // Non-existent direct pairs should NOT be found
  // Skipping a level: A0 -> C2 (must go A0->B1->C2)
  if constexpr (find_function::to_child<const DummyMatrix<float, A0, A0>&, DummyMatrix<float, A0, C2>&, const A&, const B&>) {
    t.assert_true(false, __LINE__);
  }
  // Skipping two levels: A0 -> B3
  if constexpr (find_function::to_child<const DummyMatrix<float, A0, A0>&, DummyMatrix<float, A0, B3>&, const A&, const B&>) {
    t.assert_true(false, __LINE__);
  }
  // Wrong direction: B1 -> A0 (to_parent exists, but not to_child)
  if constexpr (find_function::to_child<const DummyMatrix<float, A0, B1>&, DummyMatrix<float, A0, A0>&, const A&, const B&>) {
    t.assert_true(false, __LINE__);
  }
  // Sibling: B3 -> A3 (both children of C2, but no direct to_child between them)
  if constexpr (find_function::to_child<const DummyMatrix<float, A0, B3>&, DummyMatrix<float, A0, A3>&, const A&, const B&>) {
    t.assert_true(false, __LINE__);
  }
  // No to_child defined from A1 at all
  if constexpr (find_function::to_child<const DummyMatrix<float, A0, A1>&, DummyMatrix<float, A0, A2>&, const A&, const B&>) {
    t.assert_true(false, __LINE__);
  }
  // Wrong geometry types
  if constexpr (find_function::to_child<const DummyMatrix<float, A0, A0>&, DummyMatrix<float, A0, B1>&, const B&, const A&>) {
    t.assert_true(false, __LINE__);
  }
});

Test<"testfind_common_ancestor">([]<typename T>([[maybe_unused]]T &t) {
  if constexpr (!std::is_same_v<decltype(find_common_ancestor<B2, B3>()), A0>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (std::is_same_v<decltype(find_common_ancestor<B2, B3>()), B1>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!std::is_same_v<decltype(find_common_ancestor<A0, A0>()), A0>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!std::is_same_v<decltype(find_common_ancestor<B2, B3>()), A0>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!std::is_same_v<decltype(find_common_ancestor<A3, B3>()), C2>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!std::is_same_v<decltype(find_common_ancestor<A1, B3>()), A0>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!std::is_same_v<decltype(find_common_ancestor<B2, B2>()), B2>) {
    t.assert_true(false, __LINE__);
  }
});

Test<"testUp">([]<typename T>(T &t) {
  {
    DummyMatrix<float, A2, A0> m = up<float, A0>(DummyMatrix<float, A2, A2>(), A(), B());
    std::vector<std::string> expectedTrace{"A2_to_A1", "A1_to_A0"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
        t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  {
    DummyMatrix<float, A2, A1> m = up<float, A1>(DummyMatrix<float, A2, A2>(), A(), B());
    std::vector<std::string> expectedTrace{"A2_to_A1"};    
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }
  
  {
    DummyMatrix<float, A1, A0> m = up<float, A0>(DummyMatrix<float, A1, A1>(), A(), B());
    std::vector<std::string> expectedTrace{"A1_to_A0"};

    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }
  
  {
    DummyMatrix<float, A0, A0> m1;
    m1.trace.push_back("no_trace");
    DummyMatrix<float, A0, A0> m = up<float, A0>(m1, A(), B());
    std::vector<std::string> expectedTrace = m1.trace;
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);

    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }
}
);

Test<"testDown">([]<typename T>(T &t) {
  {
    DummyMatrix<float, A0, B3> m = down<float, B3, A0>(DummyMatrix<float, A0, A0>(), A(), B());
    std::vector<std::string> expectedTrace{"A0_to_B1", "B1_to_C2", "C2_to_B3"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  {
    DummyMatrix<float, A0, A3> m = down<float, A3, A0>(DummyMatrix<float, A0, A0>(), A(), B());
    std::vector<std::string> expectedTrace{"A0_to_B1", "B1_to_C2", "C2_to_A3"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  {
    DummyMatrix<float, B1, C2> m = down<float, C2, B1>(DummyMatrix<float, B1, B1>(), A(), B());
    std::vector<std::string> expectedTrace{"B1_to_C2"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  {
    DummyMatrix<float, A0, A0> m1;
    m1.trace.push_back("no_trace");
    DummyMatrix<float, A0, A0> m = down<float, A0, A0>(m1, A(), B());
    std::vector<std::string> expectedTrace = m1.trace;
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }
});

Test<"testrelate_systems">([]<typename T>(T &t) {
  {
    DummyMatrix<float, A2, B3> m = relate_systems<float, A2, B3, DummyMatrix>(A(), B());
    std::vector<std::string> expectedTrace{"A2_to_A1", "A1_to_A0", "A0_to_B1", "B1_to_C2", "C2_to_B3"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  {
    DummyMatrix<float, A2, A2> m = relate_systems<float, A2, A2, DummyMatrix>(A(), B());
    std::vector<std::string> expectedTrace;
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  {
    DummyMatrix<float, A0, B3> m = relate_systems<float, A0, B3, DummyMatrix>(A(), B());
    std::vector<std::string> expectedTrace{"A0_to_B1", "B1_to_C2", "C2_to_B3"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  {
    DummyMatrix<float, A2, A0> m = relate_systems<float, A2, A0, DummyMatrix>(A(), B());
    std::vector<std::string> expectedTrace{"A2_to_A1", "A1_to_A0"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }
});

Test<"testFindCommonAncestorEdgeCases">([](auto& t) {
  // Leaf to root
  t.assert_true((std::is_same_v<decltype(find_common_ancestor<A2, A0>()), A0>), __LINE__);

  // Root to leaf (reversed -- verify symmetry)
  t.assert_true((std::is_same_v<decltype(find_common_ancestor<A0, A2>()), A0>), __LINE__);

  // Same-level nodes on different branches with different parents: B2 and C2
  t.assert_true((std::is_same_v<decltype(find_common_ancestor<B2, C2>()), A0>), __LINE__);

  // Direct parent-child pair: A1 and A2
  t.assert_true((std::is_same_v<decltype(find_common_ancestor<A1, A2>()), A1>), __LINE__);

  // Symmetry: order shouldn't matter
  t.assert_true((std::is_same_v<decltype(find_common_ancestor<B3, A1>()), A0>), __LINE__);
  t.assert_true((std::is_same_v<decltype(find_common_ancestor<A1, B3>()), A0>), __LINE__);
});

Test<"testRelateSystemsEdgeCases">([](auto& t) {
  // Down-only to a different leaf than existing test (A0 -> A3 instead of A0 -> B3)
  {
    DummyMatrix<float, A0, A3> m = relate_systems<float, A0, A3, DummyMatrix>(A(), B());
    std::vector<std::string> expectedTrace{"A0_to_B1", "B1_to_C2", "C2_to_A3"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  // Up-only single step (A2 -> A1)
  {
    DummyMatrix<float, A2, A1> m = relate_systems<float, A2, A1, DummyMatrix>(A(), B());
    std::vector<std::string> expectedTrace{"A2_to_A1"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  // Root identity: A0 -> A0
  {
    DummyMatrix<float, A0, A0> m = relate_systems<float, A0, A0, DummyMatrix>(A(), B());
    t.template assert<std::equal_to<>>(m.trace.size(), (size_t)0, __LINE__);
  }
});
}