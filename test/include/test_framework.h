#pragma once

#include <bit>
#include <string>
#include <vector>
#include <iomanip>
#include <variant>
#include <algorithm>
#include <typeinfo>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <cstdlib>

#define ESC "\033["
#define LIGHT_BLUE "\033[106m"
#define PURPLE "\033[35m"
#define FAIL "\033[31m"
#define SUCCESS "\033[32m"
#define RESET "\033[m"

template<typename T>
struct near {
  static_assert(std::is_same_v<T, float>, "near<T> only supports float");

  constexpr bool operator()(T l, T r) const {
    const int32_t maxUlps{4};
    auto aInt = std::bit_cast<int32_t>(l);
    if (aInt < 0)
      aInt = INT32_MIN - aInt;
    auto bInt = std::bit_cast<int32_t>(r);
    if (bInt < 0)
      bInt = INT32_MIN - bInt;
    return std::abs(aInt - bInt) <= maxUlps;
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
      std::_Exit(1);
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
  void assert_type_among(int line, std::string types= "") {
    types += std::string(typeid(First).name()) + ", ";
    if constexpr (std::is_same_v<Type, First>){
      return;
    } else
    if constexpr (sizeof...(Among) == 0) {
      state = Failure();
      log.push_back({"FAIL: assert type among", "Line " + std::to_string(line), std::string(typeid(Type).name()) + " not among " + "{" + types + "}"});
      return;
    } else {
      assert_type_among<Type, Among...>(line, types);
    }
  }
};
