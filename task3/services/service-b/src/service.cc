#include "price/service.h"

#include <crow.h>

#include <memory>
#include <utility>

#include "events/producer.h"

namespace aleksandrit::price {

namespace {
const std::string JSON_SUCCESS = "{\"status\": \"success\"}";
const std::string JSON_SUCCESS_HEALTH = "{\"status\": true, \"service\": \"aleksandrit-price\"}";
}  // namespace

Service::Service(int port, bool debug) : port_(port), debug_(debug) {
}

Service::~Service() = default;

void Service::start() {
  crow::SimpleApp app;
  app.loglevel(debug_ ? crow::LogLevel::Debug : crow::LogLevel::Info);

  CROW_ROUTE(app, "/health").methods(crow::HTTPMethod::Get)([]() {
    return crow::response(200, "application/json", JSON_SUCCESS_HEALTH);
  });

  CROW_ROUTE(app, "/price").methods(crow::HTTPMethod::Post)([this](const crow::request& request) {
    return crow::response(200, "application/json", JSON_SUCCESS);
  });

  app.port(port_).multithreaded().run();
}

}  // namespace aleksandrit::price
