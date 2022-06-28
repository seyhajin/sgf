#pragma once

#include "std.h"

namespace sgf {

template <class FunTy> class Function;

template <class RetTy, class... ArgTys> class Function<RetTy(ArgTys...)> {

	struct Rep {
		mutable int refs = 1;

		Rep() = default;
		virtual ~Rep() = default;

		virtual RetTy invoke(const ArgTys...) = 0;
	};

	template <class FunTy> struct FunRep : public Rep {
		FunTy fun;

		explicit FunRep(const FunTy& fun) : fun(fun) {
		}

		RetTy invoke(ArgTys... args) override {
			return fun(args...);
		}
	};

	Rep* m_rep = nullptr;

	void retain() const {
		if (m_rep) ++m_rep->refs;
	}

	void release() {
		if (!m_rep || --m_rep->refs) return;
		delete m_rep;
		m_rep = nullptr;
	}

public:
	Function() = default;

	template <class FunTy> Function(const FunTy& fun) : m_rep(new FunRep<FunTy>(fun)) { // NOLINT
	}

	Function(const Function& f) : m_rep(f.m_rep) {
		retain();
	}

	Function(Function&& f) noexcept : m_rep(f.m_rep) {
		f.m_rep = nullptr;
	}

	~Function() {
		release();
	}

	template <class FunTy> Function& operator=(FunTy fun) {
		release();
		m_rep = new FunRep<FunTy>(fun);
		return *this;
	}

	Function& operator=(const Function& f) { // NOLINT
		f.retain();
		release();
		m_rep = f.m_rep;
		return *this;
	}

	Function& operator=(Function&& f) noexcept {
		if (m_rep == f.m_rep) return *this;
		release();
		m_rep = f.m_rep;
		f.m_rep = nullptr;
		return *this;
	}

	template <class... Args> RetTy operator()(Args&&... args) const {
		return m_rep->invoke(std::forward<Args>(args)...);
	}

	bool operator==(const Function& that) const {
		return m_rep == that.m_rep;
	}

	bool operator!=(const Function& that) const {
		return m_rep != that.m_rep;
	}

	bool operator<(const Function& that) const {
		return m_rep != that.m_rep;
	}

	explicit operator bool() const {
		return m_rep != nullptr;
	}
};

template <class RetTy, class... ArgTys, class FunTy> Function<RetTy(ArgTys...)> function(const FunTy& fun) {
	return Function<RetTy(ArgTys...)>(fun);
}

template <class RetTy, class... ArgTys, class InstTy, class ClassTy>
Function<RetTy(ArgTys...)> function(InstTy* inst, RetTy (ClassTy::*fun)(ArgTys...)) {
	return [inst, fun](ArgTys&&... args) -> RetTy { //
		return (inst->*fun)(std::forward<ArgTys>(args)...);
	};
}

} // namespace sgf
