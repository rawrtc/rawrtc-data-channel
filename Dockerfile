FROM ubuntu:bionic
LABEL maintainer="Lennart Grahl <lennart.grahl@gmail.com>"

# Fetch RAWRTC dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update -qqy \
 && apt-get install -qqy --no-install-recommends \
    ca-certificates \
    clang \
    cmake \
    git \
    libc-dev \
    libssl-dev \
    pkg-config \
    make \
 && rm -rf /var/lib/apt/lists/* /var/cache/apt/*

# Set working directory
WORKDIR /rawrtcdc

# Copy the code
COPY CMakeLists.txt .
COPY docs ./docs
COPY src ./src
COPY make-dependencies.sh .

# Set pkg-config path, dynamic library path and binary path (since we prefix)
ENV PKG_CONFIG_PATH=/rawrtcdc/build/prefix/lib/pkgconfig:${PKG_CONFIG_PATH}
ENV LD_LIBRARY_PATH=/rawrtcdc/build/prefix/lib:${LD_LIBRARY_PATH}
ENV PATH=/rawrtcdc/build/prefix/bin:${PATH}

# Build with clang
ENV CC=clang

# Build required dependencies
RUN ./make-dependencies.sh \
 && cd build \
 && cmake \
    -DCMAKE_INSTALL_PREFIX=/rawrtcdc/build/prefix \
    -DCMAKE_C_FLAGS="-Werror -Wall -Wno-unused-function" \
    .. \
 && make install
