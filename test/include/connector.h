#pragma once

#include <vector>
#include <string>
#include <type_traits>
#include <iostream>

template <typename From, typename To>
struct Connector {

  Connector() {
    if constexpr (std::is_same_v<From, To>) {
      log_message("identity");
    }
  }

  void log_message(const std::string& message) {
    log.push_back(message);
  }

  void log_message(const std::vector<std::string>& messages) {
    for (const auto& message : messages) {
      log_message(message);
    }
  }

  std::vector<std::string> get_log() const {
    return log;
  }

  void print_log() const {
    for (const auto& message : log) {
      std::cout << message << " ";
    }
    std::cout << std::endl;
  }

  std::vector<std::string> log;
};

template<typename From, typename Intermediate, typename To>
Connector<From, To> operator*(const Connector<Intermediate, To>& lhs, const Connector<From, Intermediate>& rhs) {

  Connector<From, To> c;
  c.log_message(rhs.get_log());
  c.log_message(" * ");
  c.log_message(lhs.get_log());
  return c;
}