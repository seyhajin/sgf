#!/bin/bash

CHECKOUT_URL="https://github.com/curl/curl.git"

CHECKOUT_DIR="$PWD/curl-checkout"

BUILD_DIR="$PWD/curl-build/release"

if test -d "$WINDIR" ; then
	export CMAKE_GENERATOR="Visual Studio 17 2022"
  BUILD_CONFIG="-DCMAKE_BUILD_TYPE=Release -DBUILD_CURL_EXE=0 -DCURL_USE_SCHANNEL=1 \
-DBUILD_SHARED_LIBS=0 -DCURL_STATIC_CRT=1 -DCURL_ZLIB=1 -DHTTP_ONLY=1 -DCURL_USE_LIBSSH=0 -DCURL_USE_LIBSSH2=0"
	BUILD_OPTS="--config Release"
else
	export CMAKE_GENERATOR="Ninja"
  BUILD_CONFIG="-DCMAKE_BUILD_TYPE=Release -DBUILD_CURL_EXE=0 \
-DBUILD_SHARED_LIBS=0 -DHTTP_ONLY=1 -DCURL_USE_LIBSSH=0 -DCURL_USE_LIBSSH2=0"
	BUILD_OPTS=""
fi

if test ! -f "$CHECKOUT_DIR/.okay" ; then
	echo "Checking out $CHECKOUT_URL to $CHECKOUT_DIR..."
	rm -rf "$CHECKOUT_DIR"
	git clone "$CHECKOUT_URL" "$CHECKOUT_DIR" || exit 1
	touch "$CHECKOUT_DIR/.okay"
fi

if test ! -f "$BUILD_DIR/.okay" ; then
	echo "Generating build dir $BUILD_DIR with ${BUILD_CONFIG}..."
	rm -rf "$BUILD_DIR"
	cmake -S "$CHECKOUT_DIR" -B "$BUILD_DIR" $BUILD_CONFIG || exit 1
	touch "$BUILD_DIR/.okay"
fi

echo "Building..."
cmake --build "$BUILD_DIR" $BUILD_OPTS || exit 1
echo "Done."
