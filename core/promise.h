#pragma once

#include "appevent.h"
#include "function.h"

#include <atomic>
#include <mutex>

namespace sgf {

template <class ValueTy> class Promise;
template <class ValueTy> using CPromise = const Promise<ValueTy>&;

template <class T> struct is_promise_base : std::false_type {};
template <class T> struct is_promise_base<Promise<T>> : public std::true_type {};
template <class T> struct is_promise : public is_promise_base<std::remove_cv_t<T>> {};
template <class T> static inline constexpr bool is_promise_v = is_promise<T>::value;

template <class ValueTy> class Promise {

	using ResolveFun = Function<void(ValueTy)>;

	struct Rep {
		std::atomic_int refs = 1;
		ResolveFun resolveFun;
	};

	Rep* m_rep = new Rep;

	void retain() {
		assert(m_rep);
		++m_rep->refs;
	}

	void release() {
		if (m_rep && !--m_rep->refs) delete m_rep;
	}

public:
	using value_type = ValueTy;

	Promise() = default;

	Promise(const Promise& promise) : m_rep(promise.m_rep) {
		retain();
	}

	Promise(Promise&& promise) noexcept : m_rep(promise.m_rep) {
		promise.m_rep = nullptr;
	}

	~Promise() {
		release();
	}

	Promise& operator=(const Promise& that) {
		that.retain();
		release();
		m_rep = that.m_rep;
		return *this;
	}

	Promise& operator=(Promise&& that) noexcept {
		if (m_rep == that.m_rep) return *this;
		release();
		m_rep = that.m_rep;
		that.m_rep = nullptr;
		return *this;
	}

	void resolve(ValueTy value) {
		if(!m_rep->resolveFun) return;
		m_rep->resolveFun(value);
	}

	void resolveAsync(ValueTy value) {
		if(!m_rep->resolveFun) return;
		postAppEvent([promise = *this, value] { promise.m_rep->resolveFun(value); });
	}

	// 'then' function that returns a promise.
	//
	template <class ThenTy, class RetTy = typename std::invoke_result_t<ThenTy, ValueTy>,
			  class ResultTy = typename RetTy::value_type, std::enable_if_t<is_promise_v<RetTy>, bool> = true>
	Promise<ResultTy> then(ThenTy thenFun) {
		assert(!m_rep->resolveFun);
		Promise<ResultTy> promise;
		m_rep->resolveFun = ResolveFun([thenFun, promise](ValueTy value) {
			thenFun(value).then([promise](ResultTy result) mutable { promise.resolve(result); });
		});
		return promise;
	}

	// 'then' function that returns void.
	//
	template <class ThenTy, class RetTy = typename std::invoke_result_t<ThenTy, ValueTy>,
			  std::enable_if_t<std::is_void_v<RetTy>, bool> = true>
	void then(ThenTy thenFun) {
		assert(!m_rep->resolveFun);
		m_rep->resolveFun = ResolveFun([thenFun](ValueTy value) mutable { thenFun(value); });
	}
};

// Operator '|' for promise | fun -> promise
//
template <class ValueTy, class FunTy, class RetTy = typename std::invoke_result_t<FunTy, ValueTy>,
		  std::enable_if_t<is_promise_v<RetTy>, bool> = true>
RetTy operator|(Promise<ValueTy> promise, FunTy fun) {
	return promise.then(fun);
}

// Operator '|' for promise | fun -> void
//
template <class ValueTy, class FunTy, class RetTy = typename std::invoke_result_t<FunTy, ValueTy>,
		  std::enable_if_t<std::is_void_v<RetTy>, bool> = true>
void operator|(Promise<ValueTy> promise, FunTy fun) {
	promise.then(fun);
}

} // namespace sgf
