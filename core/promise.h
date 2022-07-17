#pragma once

#include "function.h"

namespace sgf {

template <class ValueTy> class Promise {

	using FunTy = Function<void(ValueTy)>;

	struct Rep {
		uint refs = 1;
		bool valueValid = false;
		ValueTy value;
		FunTy thenFun;

		Rep() = default;
		Rep(ValueTy value) : value(value), valueValid(true) {
		}
	};

	mutable Rep* m_rep = nullptr;

public:
	template <class T> struct is_promise : std::false_type {};
	template <class T> struct is_promise<Promise<T>> : public std::true_type {};

	Promise() : m_rep(new Rep()) {
	}

	Promise(ValueTy value) : m_rep(new Rep(value)) {
	}

	Promise(const Promise& promise) : m_rep(promise.m_rep) {
		++m_rep->refs;
	}

	Promise(Promise&& promise) noexcept : m_rep(promise.m_rep) {
		promise.m_rep = nullptr;
	}

	~Promise() {
		if (m_rep && !--m_rep->refs) delete m_rep;
	}

	Promise& operator=(const Promise& that) {
		++that.m_rep->refs;
		if (!--m_rep->refs) delete m_rep;
		m_rep = that.m_rep;
		return *this;
	}

	Promise& operator=(Promise&& that) noexcept {
		m_rep = that.m_rep;
		that.m_rep = nullptr;
		return *this;
	}

	void fulfill(ValueTy value) {
		assert(!m_rep->valueValid);

		m_rep->value = value;
		m_rep->valueValid = true;

		if (m_rep->thenFun) m_rep->thenFun(value);
	}

	template <class RetTy> Promise<RetTy> then(Function<Promise<RetTy>(ValueTy)> fun) {
		assert(!m_rep->thenFun);

		if (m_rep->valueValid) {
			m_rep->thenFun = [](const ValueTy&) {};
			return fun(m_rep->value);
		}

		Promise<RetTy> promise;
		m_rep->thenFun = [promise, fun](ValueTy value) mutable {
			fun(value).then([promise](RetTy* result) mutable { promise.fulfill(result); });
		};
		return promise;
	}

	template <class RetTy> Promise<RetTy> then(Function<RetTy(ValueTy)> fun) {
		assert(!m_rep->thenFun);

		if (m_rep->valueValid) {
			m_rep->thenFun = [](const ValueTy&) {};
			return Promise<RetTy>(fun(m_rep->value));
		}

		Promise<RetTy> promise;
		m_rep->thenFun = [promise, fun](ValueTy value) mutable { promise.fulfill(fun(value)); };
		return promise;
	}

	template <class FunTy, class RetTy = typename std::invoke_result<FunTy, ValueTy>::type>
	Promise<RetTy> then(FunTy thenFun) {
		return then(Function<RetTy(ValueTy)>(thenFun));
	}
};

} // namespace sgf
