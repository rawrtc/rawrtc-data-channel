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
USRSCTP_COMMIT="eabbea6cfb4dac224e082fc021b568ec9a731c0b"
USRSCTP_PATH="usrsctp"

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

# Build usrsctp
if [ -z "$SKIP_USRSCTP" ]; then
    cd ${USRSCTP_PATH}
    if [ ! -d "build" ]; then
        mkdir build
    fi
    cd build
    echo "Configuring usrsctp"
    # TODO: Disable "-Wno-address-of-packed-member" once usrsctp has fixed this
    CFLAGS="-fPIC -Wno-unknown-warning-option -Wno-address-of-packed-member" \
    # SCTP_DEBUG: We need this since this is a compile time flag in RAWRTCDC
    # THREAD_SUPPORT: We explicitly don't want any threads running
    # SCTP_WITH_NO_CSUM: There's no need to verify the payload checksum since DTLS already does
    #                    that.
    cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} \
    -DSCTP_DEBUG=ON \
    -DTHREAD_SUPPORT=OFF \
    -DSCTP_WITH_NO_CSUM=OFF \
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
    EXTRA_CFLAGS="-Werror${clang_extra_cflags}" \
    ${re_make} info
    echo "Building libre"
    EXTRA_CFLAGS="-Werror${clang_extra_cflags}" \
    ${re_make} install
    rm -f ${PREFIX}/lib/libre.so ${PREFIX}/lib/libre.*dylib
    cd ${MAIN_DIR}
fi
