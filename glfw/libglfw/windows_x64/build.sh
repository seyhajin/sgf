
REPO_URL=https://github.com/blitz-research/glfw.git

CHECKOUT_DIR=$PWD/glfw-checkout
BUILD_DIR=$PWD/glfw-build/release

if test ! -z "$WINDIR" ; then
	echo "Building for Windows..."
	BUILD_CONFIG="-DGLFW_BUILD_EXAMPLES=0 -DGLFW_BUILD_TESTS=0 \
-DGLFW_BUILD_DOCS=0 -DGLFW_INSTALL=0 -GVisual\ Studio\ 17\ 2022"
	BUILD_TARGET="--target ALL_BUILD --config Release"
else
	echo "Building for Linux..."
	BUILD_CONFIG="-DCMAKE_BUILD_TYPE=Release -DGLFW_BUILD_EXAMPLES=1 -DGLFW_BUILD_TESTS=0 \
-DGLFW_BUILD_DOCS=0 -DGLFW_INSTALL=0 -G Ninja"
	BUILD_TARGET="--target all"
fi


if test ! -d $CHECKOUT_DIR ; then
	echo "Checking out $REPO_URL to $CHECKOUT_DIR..."
	git clone "$REPO_URL" "$CHECKOUT_DIR" || exit 1
fi

cd $CHECKOUT_DIR

if test ! -d $BUILD_DIR ; then
	echo "Generating build dir $BUILD_DIR with ${BUILD_CONFIG}..."
	cmake -S . -B"$BUILD_DIR" "$BUILD_CONFIG" || exit 1
fi

echo "Building $BUILD_TARGET..."
cmake --build "$BUILD_DIR" $BUILD_TARGET || exit 1
echo "Done."
