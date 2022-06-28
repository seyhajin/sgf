#include "shared.h"

namespace sgf {

namespace {
// so instance is never nullptr...
SharedPtrPool g_ptrPool;
} // namespace

SharedPtrPool* SharedPtrPool::g_instance = &g_ptrPool;

SharedPtrPool::SharedPtrPool() : m_prevInstance(g_instance) {
	g_instance = this;
}

SharedPtrPool::~SharedPtrPool() {

	while (!m_sharedPtrs.empty()) {

		auto shared = m_sharedPtrs.back();
		m_sharedPtrs.pop_back();

		// Have to be a bit careful, could cause other shared ptrs to be released
		if (!--shared->m_sharedRefs) delete shared;
	}

	g_instance = m_prevInstance;
}

} // namespace sgf
