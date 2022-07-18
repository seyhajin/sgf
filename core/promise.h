#pragma once

#include "appevent.h"
#include "function.h"

#include <atomic>
#include <mutex>

namespace sgf {

template <class ValueTy> class Promise;
template <class ValueTy> using CPromise = const Promise<ValueTy>&;

template <class ValueTy> class Promise {

	using FunTy = Function<void(const ValueTy&)>;

	struct Rep {
		std::atomic_int refs = 1;
		std::mutex mutex;
		bool resolved = false;
		ValueTy value;
		FunTy fun;

		Rep() = default;
		Rep(ValueTy value) : resolved(true), value(value) {
		}
	};

	mutable Rep* m_rep = nullptr;

	void retain() {
		assert(m_rep);
		++m_rep->refs;
	}

	void release() {
		if (!m_rep || --m_rep->refs) return;
		delete m_rep;
		m_rep = nullptr;
	}

public:
	using CValueTy = const ValueTy&;

	template <class T> struct is_promise : std::false_type {};
	template <class T> struct is_promise<Promise<T>> : public std::true_type {};
	template <class T> static inline constexpr bool is_promise_v = is_promise<T>::value;

	Promise() : m_rep(new Rep()) {
	}

	Promise(ValueTy value) : m_rep(new Rep(value)) {
	}

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

	// Can be called by any thread.
	//
	void resolve(CValueTy value) {

		std::lock_guard<std::mutex> lock(m_rep->mutex);

		assert(!m_rep->resolved);

		m_rep->resolved = true;
		m_rep->value = value;

		if (m_rep->fun) m_rep->fun(value);
	}

	// Should be called by main thread.
	//
	// The 'then' function is never called immediately, it is always called via postAsyncEvent()
	template <class RetTy> Promise<RetTy> then(Function<Promise<RetTy>(ValueTy)> thenFun) {

		std::lock_guard<std::mutex> lock(m_rep->mutex);

		assert(!m_rep->fun);

		Promise<RetTy> promise;

		m_rep->fun = [promise, thenFun](CValueTy value) {
			// clang-format off
			postAppEvent([promise, thenFun, value] {
				thenFun(value).then([promise](RetTy result) mutable {
					promise.resolve(result);
				});
			});
			// clang-format on
		};

		if (m_rep->resolved) m_rep->fun(m_rep->value);

		return promise;
	}

	template <class RetTy> Promise<RetTy> then(Function<RetTy(ValueTy)> thenFun) {

		std::lock_guard<std::mutex> lock(m_rep->mutex);

		assert(!m_rep->fun);

		Promise<RetTy> promise;

		m_rep->fun = [promise, thenFun](CValueTy value) {
			// clang-format off
			postAppEvent([promise, thenFun, value] () mutable {
				promise.resolve(thenFun(value));
			});
			// clang-format on
		};

		if (m_rep->resolved) m_rep->fun(m_rep->value);

		return promise;
	}

	void then(Function<void(ValueTy)> thenFun) {

		std::lock_guard<std::mutex> lock(m_rep->mutex);

		assert(!m_rep->fun);

		m_rep->fun = [thenFun](CValueTy value) {
			// clang-format off
			postAppEvent([thenFun, value] () {
				thenFun(value);
			});
			// clang-format on
		};

		if (m_rep->resolved) m_rep->fun(m_rep->value);
	}

	template <class ThenFunTy, class RetTy = typename std::invoke_result<ThenFunTy, ValueTy>::type,
			  std::enable_if_t<is_promise<RetTy>::value, bool> = true>
	RetTy then(ThenFunTy thenFun) {
		return then(Function<RetTy(ValueTy)>(thenFun));
	}

	template <class ThenFunTy, class RetTy = typename std::invoke_result<ThenFunTy, ValueTy>::type,
			  std::enable_if_t<!is_promise<RetTy>::value && !std::is_void<RetTy>::value, bool> = true>
	Promise<RetTy> then(ThenFunTy thenFun) {
		return then(Function<RetTy(ValueTy)>(thenFun));
	}

	template <class ThenFunTy, class RetTy = typename std::invoke_result<ThenFunTy, ValueTy>::type,
			  std::enable_if_t<std::is_void<RetTy>::value, bool> = true>
	void then(ThenFunTy thenFun) {
		then(Function<RetTy(ValueTy)>(thenFun));
	}
};

} // namespace sgf
