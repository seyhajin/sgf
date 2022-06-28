#pragma once

#include "object.h"

namespace sgf {

template <class ObjectTy> class WeakPtr {
public:
	using CWeakPtr = const WeakPtr&;

	WeakPtr() = default;

	~WeakPtr() {
		release();
	}

	template <class SourceTy> explicit WeakPtr(SourceTy* ptr) {
		retain(ptr);
	}
	template <class SourceTy> explicit WeakPtr(const WeakPtr<SourceTy>& weakPtr) {
		retain(weakPtr.m_ptr);
	}
	template <class SourceTy> WeakPtr(WeakPtr<SourceTy>&& weakPtr) = delete;

	template <class SourceTy> WeakPtr& operator=(SourceTy* ptr) {
		if (ptr != m_ptr) retain(ptr);
		return *this;
	}
	template <class SourceTy> WeakPtr& operator=(const WeakPtr<SourceTy>& weakPtr) {
		if (weakPtr.m_ptr != m_ptr) retain(weakPtr.m_ptr);
		return *this;
	}
	template <class SourceTy> WeakPtr& operator=(const WeakPtr<SourceTy>&& weakPtr) = delete;

	explicit operator bool() const {
		return m_ptr != nullptr;
	}

	bool operator==(ObjectTy* ptr) const {
		return m_ptr == ptr;
	}
	bool operator!=(ObjectTy* ptr) const {
		return !operator==(ptr);
	}

	bool operator==(const WeakPtr& that) const {
		return m_ptr == that.m_ptr;
	}
	bool operator!=(const WeakPtr& that) const {
		return !operator==(that);
	}

	ObjectTy* operator->() const {
		assert(m_ptr);
		return m_ptr;
	}
	ObjectTy& operator*() const {
		assert(m_ptr);
		return *m_ptr;
	}
	explicit operator ObjectTy*() const {
		return m_ptr;
	}

	ObjectTy* value() const {
		return m_ptr;
	}

private:
	ObjectTy* m_ptr = nullptr;
	Slot<>* m_del;

	template <class PtrTy> void retain(PtrTy* ptr) {
		if (m_ptr) m_ptr->deleted.disconnect(m_del);
		m_ptr = ptr;
		if (m_ptr) m_del = m_ptr->deleted.connect([this] { m_ptr = nullptr; });
	}

	void release() {
		if (m_ptr) m_ptr->deleted.disconnect(m_del);
	}
};

} // namespace sgf
