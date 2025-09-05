# Dependency time ---------------------------------------
ARG BASE_DEPENDENCY_IMAGE=cpp_builder
FROM ${BASE_DEPENDENCY_IMAGE}:latest AS kb_cpp_builder
FROM kb_cpp_builder AS service_a_dependecies

ARG build_dir=/service_a/build

ADD ./conanfile.txt /service_a/deps/conanfile.txt
COPY --from=kb_cpp_builder /deps/conanprofile.txt /service_a/deps/conanprofile.txt

RUN /root/.local/bin/conan install \
  /service_a/deps/conanfile.txt \
  --profile:all=/deps/conanprofile.txt \
  --output-folder=$build_dir/ --build=missing

# Compile time ---------------------------------------
FROM kservice_a_dependecies AS build

ARG toolchaine=$build_dir/build/linux-x86_64-gcc-release/generators/conan_toolchain.cmake

COPY . /service_a
WORKDIR $build_dir

RUN cmake .. --toolchain=$toolchaine \
  -DCMAKE_BUILD_TYPE=Release && \
  cmake --build . && \
  CTEST_OUTPUT_ON_FAILURE=TRUE cmake --build . --target test

# Run time ---------------------------------------
FROM debian:stable-slim

ARG PORT=8080
ARG DEBUG=False
ENV PORT=$PORT
ENV DEBUG=$DEBUG

RUN groupadd -r yarch06 && useradd -r -g yarch06 yarch06
USER yarch06

WORKDIR /service_a
COPY --from=build /service_a/build/PriceService .

EXPOSE $PORT
ENTRYPOINT sh -c './PriceService --port=$PORT --debug="$DEBUG"'
