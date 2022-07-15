#pragma once

#include "std.h"
#include "error.h"

namespace sgf {

template <class PodTy, class = std::enable_if<std::is_pod<PodTy>::value>> class PodVector;

template <class PodTy> using CPodVector = const PodVector<PodTy>&;

template <class PodTy, class> class PodVector {
public:
	using value_type = PodTy;
	using iterator = PodTy*;
	using const_iterator = const PodTy*;
	using reference = PodTy&;
	using const_reference = const PodTy&;
	using size_type = uint;
	using difference_type = int;

	PodVector() = default;

	PodVector(const PodVector& v) {
		resize(v.size());
		if (size()) std::memcpy(m_data->data, v.m_data->data, v.m_data->size * sizeof(PodTy));
	}

	PodVector(PodVector&& v) : m_data(v.m_data) {
		v.m_data = nullptr;
	}

	PodVector(std::initializer_list<PodTy> init) {
		resize(init.size());
		std::memcpy(m_data->data, init.size() * sizeof(PodTy));
	}

	~PodVector() {
		free(m_data);
	}

	PodVector& operator=(const PodVector& v) {
		if (m_data == v.m_data) return *this;
		resize(v.size());
		if (size()) std::memcpy(m_data->data, v.m_data->data, v.m_data->size * sizeof(PodTy));
		return *this;
	}

	PodVector& operator=(PodVector&& v) {
		if (m_data == v.m_data) return *this;
		free(m_data);
		m_data = v.m_data;
		v.m_data = nullptr;
		return *this;
	}

	uint capacity() const {
		return m_data ? m_data->capacity : 0;
	}

	uint size() const {
		return m_data ? m_data->size : 0;
	}

	bool empty() const {
		return !m_data || !m_data->size;
	}

	PodTy* data() {
		return m_data ? m_data->data : nullptr;
	}

	const PodTy* data() const {
		return m_data ? m_data->data : nullptr;
	}

	iterator begin() {
		return m_data ? m_data->data : nullptr;
	}

	iterator end() {
		return m_data ? m_data->data + m_data->size : nullptr;
	}

	const_iterator begin() const {
		return m_data ? m_data->data : nullptr;
	}

	const_iterator end() const {
		return m_data ? m_data->data + m_data->size : nullptr;
	}

	reference operator[](uint i) {
		return m_data->data[i];
	}

	const_reference operator[](uint i) const {
		return m_data->data[i];
	}

	void reserve(uint newCapacity) {

		if (!m_data) {
			if (!newCapacity) return;
			m_data = (Data*)malloc(sizeof(Data) + sizeof(PodTy) * newCapacity);
			m_data->size = 0;
			m_data->capacity = newCapacity;
			return;
		}
		if (newCapacity <= m_data->capacity) return;

		Data* newData = (Data*)malloc(sizeof(Data) + sizeof(PodTy) * newCapacity);
		std::memcpy(newData->data, m_data->data, m_data->size * sizeof(PodTy));
		newData->size = m_data->size;
		newData->capacity = newCapacity;

		free(m_data);
		m_data = newData;
	}

	void resize(uint newSize) {
		reserve(std::max(newSize, size() * 3 / 2 + 10));
		if (m_data) m_data->size = newSize;
	}

	void clear() {
		resize(0);
	}

	void push_back(const_reference val) {
		reserve(size() + 1);
		m_data->data[m_data->size++] = val;
	}

	void pop_back() {
		assert(!empty());
		--m_data->size;
	}

	reference& front() {
		assert(!empty());
		return m_data->data[0];
	}

	const_reference& front() const {
		assert(!empty());
		return m_data->data[0];
	}

	reference back() {
		assert(!empty());
		return m_data->data[m_data->size - 1];
	}

	const_reference back() const {
		assert(!empty());
		return m_data->data[m_data->size - 1];
	}

	iterator erase(PodTy* it) {
		assert(it >= begin() && it < end());

		for (iterator p = it, e = end() - 1; p != e; ++p) p[0] = p[1];
		--m_data->size;

		return it;
	}

	iterator insert(PodTy* it, const PodTy& val) {
		assert(it >= begin() && it <= end());

		uint i = it - begin();
		resize(size() + 1);
		it = begin() + i;

		for (PodTy* e = end() - 1; e != it; --e) e[0] = e[-1];
		*it = val;

		return it;
	}

private:
	struct Data {
		uint size = 0;
		uint capacity = 0;
		PodTy data[8];
	};
	Data* m_data = nullptr;
};

} // namespace sgf
