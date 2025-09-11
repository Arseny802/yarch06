#include "order/service.h"

#include <cpr/api.h>
#include <cpr/response.h>
#include <crow.h>
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

namespace aleksandrit::order {

namespace {
opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> get_tracer() {
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
  return provider->GetTracer("aleksandrit::order");
}

opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> get_logger() {
  auto provider = opentelemetry::logs::Provider::GetLoggerProvider();
  return provider->GetLogger("service", "aleksandrit::order");
}
}  // namespace

Service::Service(int port, bool debug, std::string price_service_url)
    : port_(port), debug_(debug), price_service_url_(std::move(price_service_url)) {
}

Service::~Service() = default;

void Service::start() {
  crow::SimpleApp app;
  app.loglevel(debug_ ? crow::LogLevel::Debug : crow::LogLevel::Info);

  CROW_ROUTE(app, "/health").methods(crow::HTTPMethod::Get)([]() {
    auto span = get_tracer()->StartSpan("health check call");
    return crow::response(200,
                          "application/json",
                          "{\"status\": true, \"service\": \"aleksandrit-order\"}");
  });

  CROW_ROUTE(app, "/order").methods(crow::HTTPMethod::Get)([this](const crow::request& request) {
    auto span = get_tracer()->StartSpan("order call");

    cpr::Response response;
    {
      auto span = get_tracer()->StartSpan("price get call");
      auto ctx = span->GetContext();

      response = cpr::Get(cpr::Url(price_service_url_ + "/price"));
      CROW_LOG_INFO << "response.status_code " << response.status_code << ".";
      CROW_LOG_DEBUG << "response.text " << response.text;
      get_logger()->Info(response.status_code, ctx.trace_id(), ctx.span_id(), ctx.trace_flags());
      get_logger()->Debug(response.text, ctx.trace_id(), ctx.span_id(), ctx.trace_flags());
    }

    return crow::response(200, "application/json", response.text);
  });

  app.port(port_).multithreaded().run();
}

}  // namespace aleksandrit::order
