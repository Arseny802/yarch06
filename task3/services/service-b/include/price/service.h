#pragma once
#include <memory>
#include <string>

namespace aleksandrit::price {
class Service {
 public:
  Service(int port, bool debug = false);
  ~Service();

  void start();

 private:
  const int port_;
  const bool debug_;
};
}  // namespace aleksandrit::price
