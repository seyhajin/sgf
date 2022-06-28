#pragma once

#include "shared.h"

namespace sgf {

//! \brief The SharedPtr template class
//!
//! Used to keep Shared instances alive.
//!
//! You should only use SharedPtr when storing shared objects on the heap (ie: in globals or fields).
//!
//! Otherwise, shared objects should just be passed and returned as raw pointers and everything should just work?
//!
template <class SharedTy> class SharedPtr {
public:
	SharedPtr() = default;

	~SharedPtr() {
		release(m_ptr);
	}

	SharedPtr(SharedTy* ptr) : m_ptr(ptr) { // NOLINT (implicit OK)
		retain(m_ptr);
	}

	SharedPtr(const SharedPtr& shared) : m_ptr(shared.m_ptr) {
		retain(m_ptr);
	}

	SharedPtr(SharedPtr&& shared) noexcept : m_ptr(shared.m_ptr) {
		shared.m_ptr = nullptr;
	}

	SharedPtr& operator=(nullptr_t) {
		release(m_ptr);
		m_ptr = nullptr;
		return *this;
	}

	SharedPtr& operator=(SharedTy* ptr) {
		retain(ptr);
		release(m_ptr);
		m_ptr = ptr;
		return *this;
	}

	SharedPtr& operator=(const SharedPtr& shared) { // NOLINT (complains about self assignment)
		retain(shared.m_ptr);
		release(m_ptr);
		m_ptr = shared.m_ptr;
		return *this;
	}

	SharedPtr& operator=(SharedPtr&& shared) noexcept {
		if (m_ptr == shared.m_ptr) return *this;
		release(m_ptr);
		m_ptr = shared.m_ptr;
		shared.m_ptr = nullptr;
		return *this;
	}

	explicit operator bool() const {
		return m_ptr != nullptr;
	}

	bool operator==(const SharedTy* ptr) const {
		return m_ptr == ptr;
	}

	bool operator==(const SharedPtr& shared) const {
		return m_ptr == shared.m_ptr;
	}

	friend bool operator==(const SharedTy* ptr, const SharedPtr& shared) {
		return ptr == shared.m_ptr;
	}

	bool operator!=(const SharedTy* ptr) const {
		return m_ptr != ptr;
	}

	bool operator!=(const SharedPtr& shared) const {
		return m_ptr != shared.m_ptr;
	}

	friend bool operator!=(const SharedTy* ptr, const SharedPtr& shared) {
		return ptr != shared.m_ptr;
	}

	bool operator<(const SharedTy* ptr) const {
		return m_ptr < ptr;
	}

	bool operator<(const SharedPtr& shared) const {
		return m_ptr < shared.m_ptr;
	}

	friend bool operator<(const SharedTy* ptr, const SharedPtr& shared) {
		return ptr < shared.m_ptr;
	}

	operator SharedTy*() const { // NOLINT (implicit OK)
		return m_ptr;
	}

	SharedTy* operator->() const {
		return m_ptr;
	}

	SharedTy& operator*() const {
		return *m_ptr;
	}

	SharedTy* value() const {
		return m_ptr;
	}

private:
	SharedTy* m_ptr = nullptr;

	static inline void retain(Shared* shared) {
		assert(!shared || shared->m_sharedRefs < 10000);

		if (shared) ++shared->m_sharedRefs;
	}

	static inline void release(Shared* shared) {
		assert(!shared || shared->m_sharedRefs > 0);

		if (!shared || --shared->m_sharedRefs) return;

		shared->m_sharedRefs = 1;
		SharedPtrPool::g_instance->m_sharedPtrs.push_back(shared);
	}
};

} // namespace sgf
