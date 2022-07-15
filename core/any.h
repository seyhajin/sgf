#pragma

#include "std.h"

namespace sgf {

class Any;
using CAny = const Any&;

class Any {

	inline static int g_nextTypeId;

	struct Rep {
		int refs = 1;
		virtual ~Rep() = default;
		virtual int typeId() const = 0;
		virtual bool equals(Rep* rhs) const = 0;
	};

	template <class ValueTy> struct RepImpl : public Rep {
		inline static const int g_typeId = ++g_nextTypeId;

		ValueTy value;

		explicit RepImpl(const ValueTy& value) : value(value) {
		}

		explicit RepImpl(ValueTy&& value) noexcept : value(std::move(value)) {
		}

		int typeId() const override {
			return g_typeId;
		}

		bool equals(Rep* rhs) const override {
			if (g_typeId != rhs->typeId()) return false;
			return value == static_cast<RepImpl<ValueTy>*>(rhs)->value;
		}
	};

	Rep* m_rep = nullptr;

	void release() {
		if (m_rep && !--m_rep->refs) delete m_rep;
	}

public:
	Any() = default;

	template <class ValueTy> explicit Any(const ValueTy& value) : m_rep(new RepImpl<ValueTy>(value)) {
	}

	template <class ValueTy> explicit Any(ValueTy&& value) noexcept : m_rep(new RepImpl<ValueTy>(std::move(value))) {
	}

	Any(const Any& any) : m_rep(any.m_rep) {
		if (m_rep) ++m_rep->refs;
	}

	Any(Any&& any) noexcept : m_rep(any.m_rep) {
		any.m_rep = nullptr;
	}

	~Any() {
		release();
	}

	template <class ValueTy> Any& operator=(const ValueTy& value) {
		release();
		m_rep = new RepImpl<ValueTy>(value);
		return *this;
	}

	Any& operator=(const Any& any) {
		if (m_rep == any.m_rep) return *this;
		release();
		m_rep = any.m_rep;
		if (m_rep) ++m_rep->refs;
		return *this;
	}

	Any& operator=(Any&& any) noexcept {
		m_rep = any.m_rep;
		any.m_rep = nullptr;
		return *this;
	}

	bool operator==(const Any& any) const {
		return m_rep && any.m_rep && m_rep->equals(any.m_rep);
	}

	bool exists() const {
		return m_rep;
	}

	template <class ValueTy> bool is() const {
		return m_rep && RepImpl<ValueTy>::g_typeId == m_rep->typeId();
	}

	template <class ValueTy> const ValueTy& get() const {
		assert(is<ValueTy>());
		return static_cast<RepImpl<ValueTy>*>(m_rep)->value;
	}
};

} // namespace sgf
