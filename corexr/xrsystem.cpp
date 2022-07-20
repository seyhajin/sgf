#include "xrsystem.h"

namespace sgf {

XRSystem::XRSystem() {
	assert(!g_instance);
	g_instance = this;
}

} // namespace sgf
