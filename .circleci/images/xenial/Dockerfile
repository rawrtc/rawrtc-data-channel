FROM ubuntu:xenial

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
    cmake \
    gcc \
    libc-dev \
    libssl-dev \
    pkg-config \
    make
