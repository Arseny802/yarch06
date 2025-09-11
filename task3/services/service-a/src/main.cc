#include <opentelemetry/exporters/ostream/span_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_http.h>
#include <opentelemetry/exporters/otlp/otlp_http_exporter.h>
#include <opentelemetry/exporters/otlp/otlp_http_exporter_options.h>
#include <opentelemetry/sdk/common/global_log_handler.h>
#include <opentelemetry/sdk/trace/exporter.h>
#include <opentelemetry/sdk/trace/processor.h>
#include <opentelemetry/sdk/trace/provider.h>
#include <opentelemetry/sdk/trace/simple_processor_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/trace/tracer_provider.h>

#include <argparse/argparse.hpp>
#include <cstdlib>
#include <string>
#include <utility>

#include "order/service.h"

namespace {
struct TracerRAII {
  TracerRAII(std::string http_url, const bool debug) {
    if (debug) {
      opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogLevel(
          opentelemetry::sdk::common::internal_log::LogLevel::Debug);
    }

    auto exporter_ostream = opentelemetry::exporter::trace::OStreamSpanExporterFactory::Create();

    opentelemetry::exporter::otlp::OtlpHttpExporterOptions opts;
    opts.url = std::move(http_url);
    auto otlp_http_exporter = std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(
        new opentelemetry::exporter::otlp::OtlpHttpExporter(opts));

    auto processor_ostream =
        opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(std::move(exporter_ostream));
    auto processor_otlp_http = opentelemetry::sdk::trace::SimpleSpanProcessorFactory::Create(
        std::move(otlp_http_exporter));

    std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor>> processors;
    processors.emplace_back(std::move(processor_ostream));
    processors.emplace_back(std::move(processor_otlp_http));
    auto resource_attributes = opentelemetry::sdk::resource::ResourceAttributes{
        {"service.name", "aleksandrit::order"},
        {"service.in_debug", std::to_string(debug)}};
    auto resource = opentelemetry::sdk::resource::Resource::Create(resource_attributes);

    std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
        opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(processors),
                                                                 std::move(resource));
    opentelemetry::sdk::trace::Provider::SetTracerProvider(provider);
  }

  ~TracerRAII() {
    std::shared_ptr<opentelemetry::trace::TracerProvider> noop;
    opentelemetry::sdk::trace::Provider::SetTracerProvider(noop);
  }
};
}  // namespace

int main(int argc, char* argv[]) {
  argparse::ArgumentParser program("Aleksandrit Order Service");
  int port = 8080;
  bool debug = false;
  std::string price_service_url;
  std::string otlp_http_url;

  program.add_argument("-p", "--port")
      .help("HTTP port to listen on")
      .scan<'i', int>()
      .default_value(port)
      .store_into(port);

  program.add_argument("-d", "--debug")
      .help("Is debug mode enabled")
      .default_value(debug)
      .store_into(debug);

  program.add_argument("--price-url")
      .help("Price Service URL")
      .default_value(std::string("http://service-b:8080"))
      .store_into(price_service_url);

  program.add_argument("--otlp-http-url")
      .help("Price Service URL")
      .default_value(std::string("http://simplest-agent:4318/v1/traces"))
      .store_into(otlp_http_url);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return EXIT_FAILURE;
  }

  TracerRAII tracer_raii(std::move(otlp_http_url), debug);
  aleksandrit::order::Service(port, debug, std::move(price_service_url)).start();
  return EXIT_SUCCESS;
}
