#pragma once
#include <memory>
#include <string>

namespace aleksandrit::order {
class Service {
 public:
  Service(int port, bool debug, std::string price_service_url);
  ~Service();

  void start();

 private:
  const int port_;
  const bool debug_;
  const std::string price_service_url_;
};
}  // namespace aleksandrit::order
