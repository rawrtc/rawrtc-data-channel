#!/bin/bash
set -e

# Build all
docker build -t rawrtc/ci-image:trusty trusty
docker build -t rawrtc/ci-image:xenial xenial
docker build -t rawrtc/ci-image:bionic bionic
docker build -t rawrtc/ci-image:archlinux archlinux

# Push all
docker push rawrtc/ci-image:trusty
docker push rawrtc/ci-image:xenial
docker push rawrtc/ci-image:bionic
docker push rawrtc/ci-image:archlinux
