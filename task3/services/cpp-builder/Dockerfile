FROM gcc:14.1.0 AS cpp_builder

ADD ./conanfile.txt /deps/conanfile.txt
ADD ./conanprofile.txt /deps/conanprofile.txt

RUN apt update && apt install -y pipx cmake && \
  pipx ensurepath && pipx install conan && \
  /root/.local/bin/conan --version && \
  /root/.local/bin/conan install /deps/conanfile.txt \
  --profile:all=/deps/conanprofile.txt \
  --output-folder=/conan/ --build=missing

