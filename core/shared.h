#pragma once

#include "debug.h"
#include "object.h"
#include "std.h"

namespace sgf {

class Shared : public Object {
public:
	Shared() {
		if(g_lonelyPtrs) g_lonelyPtrs->push_back(this);
	}

	~Shared() override{
		assert(m_sharedRefs == 0);
	}

private:
	friend class SharedPtrPool;
	template <class> friend class SharedPtr;

	static Vector<Shared*>* g_lonelyPtrs;

	uint m_sharedRefs = 1;
};

class SharedPtrPool {
public:
	SharedPtrPool() : m_prevPtrs(Shared::g_lonelyPtrs) {
		Shared::g_lonelyPtrs = &m_lonelyPtrs;
	}

	~SharedPtrPool();

private:
	Vector<Shared*> m_lonelyPtrs;
	Vector<Shared*>* m_prevPtrs;
};

} // namespace sgf
