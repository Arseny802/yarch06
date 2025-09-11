#include "price/service.h"

#include <crow.h>
#include <fmt/format.h>
#include <opentelemetry/logs/logger.h>
#include <opentelemetry/logs/logger_provider.h>
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/nostd/shared_ptr.h>
#include <opentelemetry/nostd/string_view.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/trace/tracer.h>
#include <opentelemetry/trace/tracer_provider.h>

#include <memory>
#include <utility>

namespace aleksandrit::price {

namespace {
opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> get_tracer() {
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
  return provider->GetTracer("aleksandrit::price");
}

opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> get_logger() {
  auto provider = opentelemetry::logs::Provider::GetLoggerProvider();
  return provider->GetLogger("service", "aleksandrit");
}
}  // namespace

Service::Service(int port, bool debug) : port_(port), debug_(debug) {
}

Service::~Service() = default;

void Service::start() {
  crow::SimpleApp app;
  app.loglevel(debug_ ? crow::LogLevel::Debug : crow::LogLevel::Info);

  CROW_ROUTE(app, "/health").methods(crow::HTTPMethod::Get)([]() {
    auto span = get_tracer()->StartSpan("health check call");
    return crow::response(200,
                          "application/json",
                          "{\"status\": true, \"service\": \"aleksandrit-price\"}");
  });

  CROW_ROUTE(app, "/price").methods(crow::HTTPMethod::Get)([this](const crow::request& request) {
    auto span = get_tracer()->StartSpan("price call");

    int price = 10000;
    {
      auto span = get_tracer()->StartSpan("price calculation call");
      auto ctx = span->GetContext();

      price += std::rand() % 540000;
      const auto log_message = fmt::format("Price is {}.", price);

      CROW_LOG_INFO << log_message;
      get_logger()->Info(log_message, ctx.trace_id(), ctx.span_id(), ctx.trace_flags());
    }

    return crow::response(200,
                          "application/json",
                          fmt::format("{{\"status\": \"success\", \"price\": {0}}}", price));
    ;
  });

  app.port(port_).multithreaded().run();
}

}  // namespace aleksandrit::price
