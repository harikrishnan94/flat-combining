FROM gcc:latest
RUN apt update
RUN DEBIAN_FRONTEND=noninteractive apt install -y git pkg-config gdb wget htop vim

# Install Latest Clang tools
RUN apt update && apt install -y lsb-release wget software-properties-common
RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

# Perquisites for vcpkg, cmake and ninja
RUN apt install -y curl zip unzip tar

# Install CMake
ARG CMAKE_VERSION=3.21.2
RUN cd /tmp && \
    wget -c --no-check-certificate \
    https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh \
    -O cmake.sh && \
    chmod +x cmake.sh && ./cmake.sh --prefix=/usr/local --skip-license && rm cmake.sh

# Install Ninja
ARG NINJA_VERSION=1.10.2
RUN cd /tmp && wget -c --no-check-certificate \
    https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip -O ninja.zip && \
    unzip ninja.zip && rm ninja.zip && chmod +x ninja && mv ninja /usr/local/bin

# Install cmake-format
RUN apt install -y python3-pip
RUN pip3 install cmake-format

# VCPKG
ARG VCPKG_VERSION=40a0fb232833dbc1b526394929f10b71627c2269
WORKDIR /root
RUN wget -c --no-check-certificate https://github.com/microsoft/vcpkg/archive/${VCPKG_VERSION}.zip -O vcpkg.zip
RUN unzip -qq vcpkg.zip && mv vcpkg-${VCPKG_VERSION} vcpkg
# Disable Certificate verification for downloading vcpkg-tools (need to fix).
RUN echo insecure >> ~/.curlrc
RUN cd vcpkg && ./bootstrap-vcpkg.sh --useSystemBinaries
RUN vcpkg/vcpkg integrate install && vcpkg/vcpkg integrate bash
ENV VCPKG_FORCE_SYSTEM_BINARIES=1

# Add vcpkg to PATH
ENV VCPKG_ROOT=/root/vcpkg
ENV VCPKG_FEATURE_FLAGS=registries
ENV PATH=$PATH:$VCPKG_ROOT

RUN vcpkg install boost-interprocess boost-align boost-lockfree doctest
RUN rm ~/.curlrc