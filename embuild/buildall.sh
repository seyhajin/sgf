
if test -z "$EMSCRIPTEN" ; then
	echo "Environment variable EMSCRIPTEN not set"
	exit 1
fi

TOOLCHAIN=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake

BUILD_DIR=$PWD/build

if test ! -d $BUILD_DIR ; then
	mkdir $BUILD_DIR
	cmake -B$BUILD_DIR -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN -DCMAKE_BUILD_TYPE=Release -GNinja ..
fi

cmake --build $BUILD_DIR
