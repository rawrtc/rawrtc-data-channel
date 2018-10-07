FROM ubuntu:trusty

RUN apt-get update -qqy

# CircleCI dependencies
RUN apt-get install -qqy --no-install-recommends \
    ca-certificates \
    git \
    gzip \
    openssh-client \
    tar

# Build dependencies
RUN apt-get install -qqy --no-install-recommends \
    autoconf \
    clang \
    cmake3 \
    curl \
    gcc \
    libc-dev \
    pkg-config \
    make

# Build OpenSSL
RUN curl -O https://www.openssl.org/source/openssl-1.1.1.tar.gz \
 && tar -zxf openssl-1.1.1.tar.gz \
 && cd openssl-1.1.1 \
 && ./config shared \
 && make \
 && make install \
 && ldconfig
