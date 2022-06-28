#pragma once

#include "function.h"
#include "object.h"
#include "signal.h" // NOLINT

namespace sgf {

template <class ValueTy> class Property : public Object {
public:
	using CValueTy = const ValueTy&;

	Signal<CValueTy, CValueTy> valueChanging; //(newValue, oldValue)
	Signal<CValueTy> valueChanged;			  //(newValue)

	Property() = default;

	Property(CValueTy value) : m_value(value) { // NOLINT
	}

	Property(const Property& prop) : m_value(prop.value) {
	}

	Property(Property&& prop) noexcept : m_value(std::move(prop.value)) {
	}

	Property& operator=(CValueTy newValue) {
		if (newValue == m_value) return *this;
		valueChanging.emit(newValue, m_value);
		m_value = newValue;
		valueChanged.emit(newValue);
		return *this;
	}

	Property& operator=(Property&& newValue) noexcept {
		if (newValue == m_value) return *this;
		valueChanging.emit(newValue, m_value);
		m_value = std::move(newValue);
		valueChanged.emit(newValue);
		return *this;
	}

	operator CValueTy() const { // NOLINT
		return m_value;
	}

	CValueTy value() const {
		return m_value;
	}

	bool operator==(CValueTy value) const {
		return m_value == value;
	}

	bool operator!=(CValueTy value) const {
		return m_value != value;
	}

	bool operator<(CValueTy value) const {
		return m_value < value;
	}

	friend std::ostream& operator<<(std::ostream& str, const Property& property) {
		return str << property.m_value;
	}

private:
	ValueTy m_value = {};
};

} // namespace sgf
