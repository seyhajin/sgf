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

	template <class FunTy> Rep* makeRep(const FunTy& fun) {
		return new FunRep<FunTy>(fun);
	}

	void retain() const {
		if (m_rep) ++m_rep->refs;
	}

	void release() {
		if (!m_rep || --m_rep->refs) return;
		delete m_rep;
		m_rep = nullptr;
	}

public:
	using FunPtrTy = RetTy (*)(ArgTys...);

	using CFunction = const Function&;

	template <class ObjTy> using MemFunTy = RetTy (ObjTy::*)(ArgTys...);

	Function() = default;

	Function(FunPtrTy fun) : m_rep(makeRep(fun)) {
	}

	template <class FunTy> Function(const FunTy& fun) : m_rep(makeRep(fun)) { // NOLINT
	}

	template <class ObjTy>
	Function(ObjTy* obj, MemFunTy<ObjTy> fun)
		: m_rep(makeRep([obj, fun](ArgTys&&... args) -> RetTy { return (obj->*fun)(std::forward<ArgTys>(args)...); })) {
	}

	Function(CFunction fun) : m_rep(fun.m_rep) {
		retain();
	}

	Function(Function&& fun) noexcept : m_rep(fun.m_rep) {
		fun.m_rep = nullptr;
	}

	~Function() {
		release();
	}

	template <class FunTy> Function& operator=(FunTy fun) {
		release();
		m_rep = new FunRep<FunTy>(fun);
		return *this;
	}

	Function& operator=(const Function& fun) { // NOLINT
		fun.retain();
		release();
		m_rep = fun.m_rep;
		return *this;
	}

	Function& operator=(Function&& fun) noexcept {
		if (m_rep == fun.m_rep) return *this;
		release();
		m_rep = fun.m_rep;
		fun.m_rep = nullptr;
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
		return m_rep < that.m_rep;
	}

	explicit operator bool() const {
		return m_rep != nullptr;
	}
};

} // namespace sgf
