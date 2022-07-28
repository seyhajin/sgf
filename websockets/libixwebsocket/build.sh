
DIR_NAME="ixwebsocket"

CHECKOUT_URL=https://github.com/blitz-research/IXWebSocket.git
CHECKOUT_DIR="$PWD/${DIR_NAME}-checkout"

BUILD_CONFIG="-DUSE_TLS=1 -DCMAKE_BUILD_TYPE=Release -G Ninja"
BUILD_DIR="$PWD/${DIR_NAME}-build/release"

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
cmake --build "$BUILD_DIR" || exit 1
echo "Done."
