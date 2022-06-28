
if test -z "$WINDIR" ; then
	exit 0
fi

REPO_URL=https://github.com/blitz-research/openal-soft.git

CHECKOUT_DIR=$PWD/openal-checkout

BUILD_DIR=$PWD/openal-build/release

BUILD_CONFIG="-DALSOFT_DLOPEN=0 -DALSOFT_UTILS=0 -DALSOFT_NO_CONFIG_UTIL=1 -DALSOFT_EXAMPLES=0 \
-DALSOFT_INSTALL=0 -ALSOFT_UPDATE_BUILD_VERSION=0 -GVisual\ Studio\ 17\ 2022"

BUILD_TARGET="--target ALL_BUILD --config Release"

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
