# Dependency time ---------------------------------------
ARG BASE_DEPENDENCY_IMAGE=cpp-builder
FROM ${BASE_DEPENDENCY_IMAGE}:latest AS task3_cpp_builder
FROM task3_cpp_builder AS service_b_dependecies

ARG build_dir=/service_b/build

ADD ./conanfile.txt /service_b/deps/conanfile.txt
COPY --from=task3_cpp_builder /deps/conanprofile.txt /service_b/deps/conanprofile.txt

RUN /root/.local/bin/conan install \
  /service_b/deps/conanfile.txt \
  --profile:all=/deps/conanprofile.txt \
  --output-folder=$build_dir/ --build=missing

# Compile time ---------------------------------------
FROM service_b_dependecies AS build

ARG toolchaine=$build_dir/build/linux-x86_64-gcc-release/generators/conan_toolchain.cmake

COPY . /service_b
WORKDIR $build_dir

RUN cmake .. --toolchain=$toolchaine \
  -DCMAKE_BUILD_TYPE=Release && \
  cmake --build . && \
  CTEST_OUTPUT_ON_FAILURE=TRUE cmake --build . --target test

# Run time ---------------------------------------
FROM debian:stable-slim

ARG PORT=8080
ARG DEBUG=False
ARG OTLP_HTTP_URL=http://simplest-agent:4318/v1/traces
ENV PORT=$PORT
ENV DEBUG=$DEBUG
ENV OTLP_HTTP_URL=$OTLP_HTTP_URL

RUN apt-get -y update && apt-get -y install wget

WORKDIR /service_b
COPY --from=build /service_b/build/PriceService .

EXPOSE $PORT
ENTRYPOINT sh -c './PriceService --port=$PORT --debug="$DEBUG" --otlp-http-url="$OTLP_HTTP_URL"'
