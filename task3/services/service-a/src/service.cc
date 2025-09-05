#include "order/service.h"

#include <crow.h>

#include <memory>
#include <utility>

#include "events/producer.h"

namespace aleksandrit::order {

namespace {
const std::string JSON_SUCCESS = "{\"status\": \"success\"}";
const std::string JSON_SUCCESS_HEALTH = "{\"status\": true, \"service\": \"aleksandrit-order\"}";
}  // namespace

Service::Service(int port, bool debug, std::string price_service_url)
    : port_(port), debug_(debug), price_service_url_(std::move(price_service_url)) {
}

Service::~Service() = default;

void Service::start() {
  crow::SimpleApp app;
  app.loglevel(debug_ ? crow::LogLevel::Debug : crow::LogLevel::Info);

  CROW_ROUTE(app, "/health").methods(crow::HTTPMethod::Get)([]() {
    return crow::response(200, "application/json", JSON_SUCCESS_HEALTH);
  });

  CROW_ROUTE(app, "/order").methods(crow::HTTPMethod::Post)([this](const crow::request& request) {
    cpr::Response original_response = cpr::Get(cpr::Url(price_service_url_));
    CROW_LOG_DEBUG << "<< original_response.status_code " << original_response.status_code << ".";
    return crow::response(200, "application/json", JSON_SUCCESS);
  });

  app.port(port_).multithreaded().run();
}

}  // namespace aleksandrit::order
