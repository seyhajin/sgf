
# Note: On Windows, you'll have to manually install zlib somewhere cmake can see it to be able to build this.

OUTPUT_NAME="ixwebsocket"
BUILD_CONFIG="-DUSE_TLS=1 -DUSE_OPEN_SSL=1 -DUSE_ZLIB=0"

CHECKOUT_URL=https://github.com/blitz-research/IXWebSocket.git
CHECKOUT_DIR="$PWD/${OUTPUT_NAME}-checkout"
BUILD_DIR="$PWD/${OUTPUT_NAME}-build/release"
BUILD_OPTS=""

if test -d "$WINDIR" ; then
	export CMAKE_GENERATOR="Visual Studio 17 2022"
	BUILD_OPTS="--config Release"
else
	BUILD_CONFIG="${BUILD_CONFIG} -DCMAKE_BUILD_TYPE=Release -G Ninja"
	build_OPTS=""
fi

if test ! -f "$CHECKOUT_DIR/.okay" ; then
	echo "Checking out $CHECKOUT_URL to $CHECKOUT_DIR..."
	rm -rf "$CHECKOUT_DIR"
	git clone "$CHECKOUT_URL" "$CHECKOUT_DIR" || exit 1
	touch "$CHECKOUT_DIR/.okay"
fi
	
if test ! -f "$BUILD_DIR/.okay" ; then
	echo "Generating build dir $BUILD_DIR with config ${BUILD_CONFIG}..."
	rm -rf "$BUILD_DIR"
	cmake -S "$CHECKOUT_DIR" -B "$BUILD_DIR" $BUILD_CONFIG || exit 1
	touch "$BUILD_DIR/.okay"
fi

echo "Building $BUILD_TARGET..."
cmake --build "$BUILD_DIR" $BUILD_OPTS || exit 1
echo "Done."
