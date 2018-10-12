#!/bin/sh
set -e

# Number of threads to use
export THREADS=12

# Library prefix
export LIB_PREFIX=rawrtc_

# Build path
if [ -z "$BUILD_PATH" ]; then
    export BUILD_PATH=${PWD}/build
fi

# Offline?
offline=false
if [ ! -z "$OFFLINE" ]; then
    offline=true
fi

# Dependencies
LIBRE_GIT="https://github.com/rawrtc/re.git"
LIBRE_BRANCH="rawrtc-patched"
LIBRE_COMMIT="7d837903ac78bbfbc56e2efb21315665d1834f1e"
LIBRE_PATH="re"
USRSCTP_GIT="https://github.com/rawrtc/usrsctp.git"
USRSCTP_BRANCH="usrsctp-for-rawrtc"
USRSCTP_COMMIT="005db8982a6304109aacffc70b16059bf17fee32"
USRSCTP_PATH="usrsctp"
RAWRTCC_GIT="https://github.com/rawrtc/rawrtc-common.git"
RAWRTCC_BRANCH="master"
RAWRTCC_PATH="rawrtcc"

# Prefix
export PREFIX=${BUILD_PATH}/prefix
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig:${PKG_CONFIG_PATH}
export CFLAGS="${CFLAGS} -I${PREFIX}/include"
export CPPFLAGS="${CFLAGS}"
export LDFLAGS="${LDFLAGS} -L${PREFIX}/lib"
echo "Environment vars:"
echo "PREFIX: ${PREFIX}"
echo "PKG_CONFIG_PATH: ${PKG_CONFIG_PATH}"
echo "CFLAGS: ${CFLAGS}"
echo "CPPFLAGS: ${CPPFLAGS}"
echo "LDFLAGS: ${LDFLAGS}"
echo ""

# Check for DTLS 1.2 support in OpenSSL
echo "OpenSSL version: `pkg-config --short-errors --modversion openssl`"
pkg-config --atleast-version=1.0.2 openssl || have_dtls_1_2=false
if [ "$have_dtls_1_2" = false ]; then
    echo "Error: OpenSSL >= 1.0.2 is required"
    exit 1
fi

# Set up build path
mkdir -p ${BUILD_PATH}/dependencies
MAIN_DIR=${BUILD_PATH}/dependencies
cd ${MAIN_DIR}

# Get platform
platform=`uname`
echo "Platform: $platform"
re_make="make"
if [ "$platform" = 'FreeBSD' ]; then
    re_make="gmake"
fi

# Extra cflags when using clang
clang_extra_cflags=""
if [ "${CC}" = "clang" ]; then
    clang_extra_cflags=" -Wno-error=unused-command-line-argument"
fi

# Get usrsctp
if [ -z "$SKIP_USRSCTP" ]; then
    if [ ! -d "${USRSCTP_PATH}" ]; then
        echo "Cloning usrsctp"
        git clone -b ${USRSCTP_BRANCH} ${USRSCTP_GIT} ${USRSCTP_PATH}
        cd ${USRSCTP_PATH}
    elif [ "$offline" = false ]; then
        cd ${USRSCTP_PATH}
        echo "Pulling usrsctp"
        git pull
    else
        cd ${USRSCTP_PATH}
    fi
    git checkout ${USRSCTP_BRANCH}
    git reset --hard ${USRSCTP_COMMIT}
    cd ${MAIN_DIR}
fi

# Get libre
if [ -z "$SKIP_LIBRE" ]; then
    if [ ! -d "${LIBRE_PATH}" ]; then
        echo "Cloning libre"
        git clone -b ${LIBRE_BRANCH} ${LIBRE_GIT} ${LIBRE_PATH}
        cd ${LIBRE_PATH}
    elif [ "$offline" = false ]; then
        cd ${LIBRE_PATH}
        echo "Pulling libre"
        git pull
    else
        cd ${LIBRE_PATH}
    fi
    git checkout ${LIBRE_BRANCH}
    git reset --hard ${LIBRE_COMMIT}
    cd ${MAIN_DIR}
fi

# Get RAWRTCC
if [ -z "$SKIP_RAWRTCC" ]; then
    if [ ! -d "${RAWRTCC_PATH}" ]; then
        echo "Cloning RAWRTCC"
        git clone -b ${RAWRTCC_BRANCH} ${RAWRTCC_GIT} ${RAWRTCC_PATH}
        cd ${RAWRTCC_PATH}
    elif [ "$offline" = false ]; then
        cd ${RAWRTCC_PATH}
        echo "Pulling RAWRTCC"
        git pull
    else
        cd ${RAWRTCC_PATH}
    fi
    git checkout ${RAWRTCC_BRANCH}
    cd ${MAIN_DIR}
fi

# Set openssl sysroot
openssl_sysroot=`pkg-config --variable=prefix openssl`
echo "Using OpenSSL sysroot: $openssl_sysroot"

# Build usrsctp
if [ -z "$SKIP_USRSCTP" ]; then
    cd ${USRSCTP_PATH}
    if [ ! -d "build" ]; then
        mkdir build
    fi
    cd build
    echo "Configuring usrsctp"
    # SCTP_DEBUG: We need this since this is a compile time flag in RAWRTCDC
    # THREAD_SUPPORT: We explicitly don't want any threads running
    CFLAGS="-fPIC" \
    cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} \
    -Dsctp_debug=ON \
    -Dsctp_thread_support=OFF \
    ..
    echo "Cleaning usrsctp"
    make clean
    echo "Building & installing usrsctp"
    make install -j${THREADS}
    rm -f ${PREFIX}/lib/libusrsctp.so* ${PREFIX}/lib/libusrsctp.*dylib
    cd ${MAIN_DIR}
fi

# Build libre
if [ -z "$SKIP_LIBRE" ]; then
    cd ${LIBRE_PATH}
    echo "Cleaning libre"
    ${re_make} clean
    echo "Build information for libre:"
    SYSROOT_ALT=${openssl_sysroot} \
    EXTRA_CFLAGS="-Werror${clang_extra_cflags}" \
    ${re_make} info
    echo "Building libre"
    SYSROOT_ALT=${openssl_sysroot} \
    EXTRA_CFLAGS="-Werror${clang_extra_cflags}" \
    ${re_make} install
    rm -f ${PREFIX}/lib/libre.so ${PREFIX}/lib/libre.*dylib
    cd ${MAIN_DIR}
fi

# Build RAWRTCC
if [ -z "$SKIP_RAWRTCC" ]; then
    cd ${RAWRTCC_PATH}
    if [ ! -d "build" ]; then
        mkdir build
    fi
    cd build
    echo "Configuring RAWRTCC"
    cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} -DBUILD_SHARED_LIBS=OFF \
    ..
    echo "Cleaning RAWRTCC"
    make clean
    echo "Building & installing RAWRTCC"
    make install -j${THREADS}
    cd ${MAIN_DIR}
fi
