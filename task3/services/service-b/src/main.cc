#include <argparse/argparse.hpp>
#include <cstdlib>
#include <string>
#include <utility>

#include "price/service.h"

int main(int argc, char* argv[]) {
  argparse::ArgumentParser program("Aleksandrit Price Service");
  int port = 8082;
  bool debug = false;

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

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return EXIT_FAILURE;
  }

  aleksandrit::price::Service(port, debug).start();
  return EXIT_SUCCESS;
}
