#include "shared.h"

namespace sgf {

Vector<Shared*>* Shared::g_lonelyPtrs;

SharedPtrPool::~SharedPtrPool() {

	assert(Shared::g_lonelyPtrs == &m_lonelyPtrs);

	Shared::g_lonelyPtrs = m_prevPtrs;

	while (!m_lonelyPtrs.empty()) {

		auto shared = m_lonelyPtrs.back();
		m_lonelyPtrs.pop_back();

		if (!--shared->m_sharedRefs) delete shared;
	}
}

} // namespace sgf
