#include <argparse/argparse.hpp>
#include <cstdlib>
#include <string>
#include <utility>

#include "order/service.h"

int main(int argc, char* argv[]) {
  argparse::ArgumentParser program("Aleksandrit Order Service");
  int port = 8082;
  bool debug = false;
  std::string price_service_url;

  program.add_argument("-p", "--port")
      .help("HTTP port to listen on")
      .scan<'i', int>()
      .default_value(port)
      .store_into(port);

  program.add_argument("-d", "--debug")
      .help("Is debug mode enabled")
      .implicit_value(true)
      .default_value(debug)
      .store_into(debug);

  program.add_argument("--price-url")
      .help("Price Service URL")
      .default_value(std::string("http://service-b:8080"))
      .store_into(price_service_url);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return EXIT_FAILURE;
  }

  aleksandrit::order::Service(port, debug, std::move(price_service_url)).start();
  return EXIT_SUCCESS;
}
