#include "ts_test.h"
#include "ts/typesafe_coordinate_systems.h"

#include <string>
#include <vector>
#include <iomanip>
#include <variant>
#include <algorithm>
#include <typeinfo>
#include <iostream>

#define ESC "\033["
#define LIGHT_BLUE "\033[106m"
#define PURPLE "\033[35m"
#define FAIL "\033[31m"
#define SUCCESS "\033[32m"
#define RESET "\033[m"

using namespace ts;

template<typename T>
struct near {
  constexpr bool operator()(T l, T r) const {
  // Code from https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
  // The machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units in the last place)
  // unless the result is subnormal.
  //int ulp;
  //if (std::is_same_v<TU_TYPE, float>) {
  //  ulp = 10;
  //} else {
  //  ulp = 100000000;
  //}
  //return (std::abs(l - r) <= std::numeric_limits<T>::epsilon() * std::abs(l + r) * ulp
  //        || std::abs(l - r) < std::numeric_limits<T>::min());

    // Make sure maxUlps is non-negative and small enough that the    
    // default NAN won't compare as equal to anything.
    const int  maxUlps{4};    
    //assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);    
    int aInt = *(int*)&l;    
    // Make aInt lexicographically ordered as a twos-complement int    
    if (aInt < 0)    
        aInt = 0x80000000 - aInt;    
    // Make bInt lexicographically ordered as a twos-complement int    
    int bInt = *(int*)&r;    
    if (bInt < 0)    
        bInt = 0x80000000 - bInt;    
    int intDiff = abs(aInt - bInt);    
    if (intDiff <= maxUlps)    
        return true;    
    return false;    
  }

};

template<size_t N>
struct String_literal {
  constexpr String_literal(const char (&str)[N]) {
      std::copy_n(str, N, name);
  }
  char name[N];
};

struct Success {};
struct Failure {};

struct Test_stats {
  static inline int fail{0};
  static inline int success{0};

  ~Test_stats() {
    std::cout << SUCCESS << "SUCCESS: " << success << RESET << std::endl;
    if (fail) {
      std::cout << FAIL    << "FAIL   : " << fail << RESET << std::endl;
    }
  }
} stats;

template<String_literal lit>
struct Test {

  struct Layout {
    int column_width{40};
  } layout;

  std::variant<Success, Failure> state;
  std::vector<std::vector<std::string>> log;

  template<typename F>
  Test(const F f) {
    try {
      f(*this);
    } catch(...) {
      std::cout << "Unexpected exception" << std::endl;
      throw;
    }
    Log();
  }

  template<typename T>
  std::string to_str(const T& t) {
    if constexpr (std::is_arithmetic_v<T>) {
      return std::to_string(t);
    }
    else if constexpr (std::is_same_v<std::string, T>) {
      return t;
    } else {
      return std::string("<not string convertible>");
    }
  } 

  void Log(){
    auto style = SUCCESS;
    if (std::holds_alternative<Success>(state)) {
       log.push_back({"SUCCESS"});
       Test_stats::success++;
    } else {
      style = FAIL;
      Test_stats::fail++;
    }
    std::cout << style << std::left << std::setw(layout.column_width) << lit.name;
    for (const auto &row : log) {
      std::cout << style;
      for(const auto &column: row){
        std::cout << std::left << std::setw(layout.column_width) << column;
      }
        std::cout << RESET;
        std::cout << std::endl << std::setfill(' ') << std::setw(layout.column_width) << " ";
    }
    std::cout << RESET << std::endl;
  }

void assert_true(bool is_true, int line) {
  if (!is_true) {
    state = Failure();
    log.push_back({"FAIL: assert_true", "Line " + std::to_string(line), ""});
  }
}

void assert_false(bool is_true, int line) {
  if (is_true) {
    state = Failure();
    log.push_back({"FAIL: assert_false", "Line " + std::to_string(line), ""});
  }
}

template<typename Op, typename T>
void assert(const T& l, const T& r, int line) {
    Op op;

 if (!op(l, r)) {
      state = Failure();
      if constexpr (std::is_same_v<Op, std::equal_to<>>) {
        log.push_back({"FAIL: assert_equal", "Line " + std::to_string(line), to_str(l) + " != " + to_str(r)});
      } else
      if constexpr (std::is_same_v<Op, near<T>>) {
        log.push_back({"FAIL: assert_near", "Line " + std::to_string(line), to_str(l) + " not near " + to_str(r)});
      } else {
        log.push_back({"FAIL: assert", "Line " + std::to_string(line)});
      }
  }
}

template<typename Type, typename First, typename ...Among>
constexpr void assert_type_among(int line, std::string types= "") {
  types += std::string(typeid(First).name()) + ", ";
  if constexpr (std::is_same_v<Type, First>){
    return;
  } else
  if constexpr (sizeof...(Among) == 0) {
    state = Failure();
    log.push_back({"FAIL: assert type among", "Line " + std::to_string(line), std::string(typeid(Type).name()) + " not among " + "{" + types + "}"});
    types="";
    return;
  } else {
    assert_type_among<Type, Among...>(line, types);
  }
}
};


int main() {

Test<"testFindFunction">([]<typename T>([[maybe_unused]]T& t) {

  if constexpr (!find_function::toChild<A, B>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::toChild<A&, B>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::toChild<const A&, B&, const C>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::toChild<const A&, const B>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::toChild<A&, B, C&, const D>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::toChild<A&, B, const C&, const D, E>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::toChild<A, B&>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::toChild<A&, B&>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::toChild<const A&, B&>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!find_function::toChild<A&, const B&>) {
    t.assert_true(false, __LINE__);
  }

  //

  if constexpr (find_function::toChild<A, C>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (find_function::toChild<const A&, C>) {
    t.assert_true(false, __LINE__);
  }

  ///

  if constexpr (find_function::toChild<A, B, C>) { //?
    t.assert_true(false, __LINE__);
  }

  if constexpr (find_function::toChild<A, const B, C>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (find_function::toChild<A, B, C&>) { //?
   t.assert_true(false, __LINE__);
  }

  ///

  if constexpr (find_function::toChild<A, B, C, D>) { //?
    t.assert_true(false, __LINE__);
  }

  if constexpr (find_function::toChild<A, B, C&, D>) { //?
    t.assert_true(false, __LINE__);
  }

  if constexpr (find_function::toChild<const A, B, C&, const D>) {
    t.assert_true(false, __LINE__);
  }

  ///

  if constexpr (find_function::toChild<A, B, C, D, E>) { //?
    t.assert_true(false, __LINE__);
  }

  if constexpr (find_function::toChild<A, B, D, E, C>) {
    t.assert_true(false, __LINE__);
  }
});

Test<"testFindCommonAncestor">([]<typename T>([[maybe_unused]]T &t) {
  if constexpr (!std::is_same_v<decltype(findCommonAncestor<B2, B3>()), A0>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (std::is_same_v<decltype(findCommonAncestor<B2, B3>()), B1>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!std::is_same_v<decltype(findCommonAncestor<A0, A0>()), A0>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!std::is_same_v<decltype(findCommonAncestor<B2, B3>()), A0>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!std::is_same_v<decltype(findCommonAncestor<A3, B3>()), C2>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!std::is_same_v<decltype(findCommonAncestor<A1, B3>()), A0>) {
    t.assert_true(false, __LINE__);
  }

  if constexpr (!std::is_same_v<decltype(findCommonAncestor<B2, B2>()), B2>) {
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

Test<"testRelateSystems">([]<typename T>(T &t) {
  {
    DummyMatrix<float, A2, B3> m = relateSystems<float, A2, B3, DummyMatrix>(A(), B());
    std::vector<std::string> expectedTrace{"A2_to_A1", "A1_to_A0", "A0_to_B1", "B1_to_C2", "C2_to_B3"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  {
    DummyMatrix<float, A2, A2> m = relateSystems<float, A2, A2, DummyMatrix>(A(), B());
    std::vector<std::string> expectedTrace;
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  {
    DummyMatrix<float, A0, B3> m = relateSystems<float, A0, B3, DummyMatrix>(A(), B());
    std::vector<std::string> expectedTrace{"A0_to_B1", "B1_to_C2", "C2_to_B3"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }

  {
    DummyMatrix<float, A2, A0> m = relateSystems<float, A2, A0, DummyMatrix>(A(), B());
    std::vector<std::string> expectedTrace{"A2_to_A1", "A1_to_A0"};
    t.template assert<std::equal_to<>>(m.trace.size(), expectedTrace.size(), __LINE__);
    for (int i = 0; i < (int)m.trace.size(); ++i) {
      t.template assert<std::equal_to<>>(m.trace[i], expectedTrace[i], __LINE__);
    }
  }
});
}