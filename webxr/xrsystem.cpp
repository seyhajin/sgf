#include "xrsystem.h"

#ifdef OS_EMSCRIPTEN
#include "webxrsystem.h"
#else
#include "openxrsystem.h"
#endif

namespace sgf {

XRSystem* createXRSystem(GraphicsDevice* graphicsDevice) {
#ifdef OS_EMSCRIPTEN
	return new WebXRSystem(graphicsDevice);
#else
	return new OpenXRSystem(graphicsDevice);
#endif
}

} // namespace sgf
