#pragma once

#include "debug.h"
#include "object.h"
#include "std.h"

namespace sgf {

#define SGF_SHARED_CLASS(CLASS)                                                                                         \
	class CLASS;                                                                                                       \
	using CLASS##Ptr = SharedPtr<CLASS>;

class Shared;
class SharedPtrPool;
template <class SharedTy> class SharedPtr;

//!
//! \brief The SharedPtrPool class
//!
//! Used to collect zombie shared pointers, ie: shared pointers with 0 refs.
//!
//! Zombies are auto-deleted when the SharedPtrPool they belong to is destropyed. Use a SharedPtr<> to keep shared
//! objects alive longer.
//!
//! Shared objects stored in SharedPtrs also be4come zombies if their ref counts reach, so they are not immediately
//! deleted.
//!
class SharedPtrPool {
public:
	SharedPtrPool();

	~SharedPtrPool();

private:
	friend class Shared;
	template <class SharedTy> friend class SharedPtr;
	static SharedPtrPool* g_instance;

	SharedPtrPool* m_prevInstance;
	Vector<Shared*> m_sharedPtrs;
};

//!
//! \brief The Shared class
//!
//! Provides a simple intrusive reference count for sharable objects.
//!
//! The only way to increment or decrement the reference count is by means of SharedPtr<>.
//!
class Shared : public Object {
public:
	Shared() {
		SharedPtrPool::g_instance->m_sharedPtrs.push_back(this);
	}

	~Shared() {
		assert(m_sharedRefs == 0);
	}

private:
	friend class SharedPtrPool;
	template <class SharedTy> friend class SharedPtr;

	uint m_sharedRefs = 1;
};

} // namespace sgf
