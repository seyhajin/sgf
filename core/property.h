#pragma once

#include "function.h"
#include "object.h"
#include "signal.h" // NOLINT

namespace sgf {

template <class ValueTy> class Property : public Object {
public:
	using CValueTy = const ValueTy&;
	using CProperty = const Property&;

	Signal<CValueTy> valueChanged; //(newValue)

	Property() = default;

	explicit Property(CValueTy value) : m_value(value) {
	}

	Property(CProperty value) = delete;//: m_value(value.m_value) {}

	Property(Property&& value) noexcept = delete; //noexcept : m_value(std::move(value.m_value)) {}

	Property& operator=(CValueTy value) {
		if (m_value == value) return *this;
		m_value = value;
		valueChanged.emit(m_value);
		return *this;
	}

	Property& operator=(CProperty value) {
		if (m_value == value.m_value) return *this;
		m_value = value.m_value;
		valueChanged.emit(m_value);
		return *this;
	}

	Property& operator=(Property&& value) noexcept {
		if (m_value == value.m_value) return *this;
		m_value = std::move(value.m_value);
		valueChanged.emit(m_value);
		return *this;
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

	operator CValueTy() const { //NOLINT
		return m_value;
	}

	CValueTy value() const {
		return m_value;
	}

	friend std::ostream& operator<<(std::ostream& str, const Property& property) {
		return str << property.m_value;
	}

private:
	ValueTy m_value{};
};

} // namespace sgf
