cmake_minimum_required(VERSION 3.23)
project(OrderService VERSION 0.0.1)
set(VENDOR "Arseny802")

option(BUILD_TESTS "Build test programs" ON)

find_package(argparse CONFIG REQUIRED)
find_package(opentelemetry-cpp REQUIRED)
find_package(Crow REQUIRED)
find_package(cpr REQUIRED)

add_library(${PROJECT_NAME}_OBJ OBJECT
  src/consumer.cc
  src/producer.cc
  src/service.cc
)
target_link_libraries(${PROJECT_NAME}_OBJ PUBLIC
  opentelemetry-cpp::opentelemetry-cpp
  Crow::Crow
  cpr::cpr
)
set_target_properties(${PROJECT_NAME}_OBJ PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)
target_include_directories(${PROJECT_NAME}_OBJ
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(${PROJECT_NAME} src/main.cc)
target_link_libraries(${PROJECT_NAME} PRIVATE 
  ${PROJECT_NAME}_OBJ
  argparse::argparse
  -static-libgcc 
  -static-libstdc++
)


if(BUILD_TESTS)
  enable_testing()
  set(PROJECT_TESTS "${PROJECT_NAME}Test")

  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE_DIRS})

  add_executable(${PROJECT_TESTS}
    tests/service.cc
  )
  target_link_libraries(${PROJECT_TESTS}
    ${PROJECT_NAME}_OBJ
    GTest::gtest_main
    GTest::gtest
    pthread
  )

  add_test(NAME "${PROJECT_TESTS}" COMMAND ${PROJECT_TESTS})
endif(BUILD_TESTS)
