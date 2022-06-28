#ifndef SGF_SIGNAL_H
#define SGF_SIGNAL_H

#include "debug.h"
#include "podvector.h"
#include "std.h"

namespace sgf {

class Object;

template <class... ArgTys> class Signal;

template <class... ArgTys> class Slot {
public:
	using SignalTy = Signal<ArgTys...>;

private:
	friend SignalTy;

	Object* context;
	Slot<>* deleter = nullptr;

	Slot(SignalTy* signal, Object* context);

	virtual ~Slot();

	virtual void invoke(ArgTys&&... args) = 0;
};

inline bool g_disconnectMe;

template <class... ArgTys> class Signal {
public:
	using SlotTy = Slot<ArgTys...>;

	Signal() = default;

	~Signal();

	template <class FunTy> SlotTy* connect(Object* context, const FunTy& fun) {

		auto slot = new SlotImpl<FunTy>(this, context, fun);

		m_slots.push_back(slot);
		return slot;
	}

	template <class ObjTy> SlotTy* connect(ObjTy* context, void (ObjTy::*fun)(ArgTys...)) {

		return connect(context, [context, fun](ArgTys&&... args) { //
			(context->*fun)(std::forward<ArgTys>(args)...);
		});
	}

	template <class FunTy> SlotTy* connect(const FunTy& fun) {

		return connect<FunTy>(nullptr, fun);
	}

	bool connected() const {
		return !m_slots.empty();
	}

	bool connected(SlotTy* slot) const {

		return std::find(m_slots.begin(), m_slots.end(), slot) != m_slots.end();
	}

	bool connected(Object* context) const {

		auto pred = [context](SlotTy* slot) { return slot && slot->context == context; };

		return std::find_if(m_slots.begin(), m_slots.end(), pred) != m_slots.end();
	}

	void disconnect(SlotTy* slot) {

		assert(connected(slot));

		auto it = std::find(m_slots.begin(), m_slots.end(), slot);
		assert(it != m_slots.end());

		*it = nullptr;
		delete slot;
	}

	void disconnect(Object* context) {

		assert(connected(context));

		for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
			auto slot = *it;
			if (!slot || slot->context != context) continue;
			*it = nullptr;
			delete slot;
		}
	}

	// It's probably unsafe to use && here?
	// What happens if 'this' is deleted mid-loop?
	template <class... Args> void emit(Args... args) {

		++m_emitting;
		auto prevDisconnectMe = g_disconnectMe;

		// Careful! Use index instead of iterator here in case
		// a new slot is added and vector is reallocated.
		for (uint i = 0, n = m_slots.size(); i < n; ++i) {

			if (auto slot = m_slots[i]) {

				g_disconnectMe = false;
				slot->invoke(std::forward<ArgTys>(args)...);

				if (m_slots[i]) {
					if (!g_disconnectMe) continue;
					m_slots[i] = nullptr;
					delete slot;
				}
			}

			if (m_emitting > 1) continue;

			m_slots.erase(m_slots.begin() + i);
			--i;
			--n;
		}

		g_disconnectMe = prevDisconnectMe;
		--m_emitting;
	}

private:
	template <class FunTy> struct SlotImpl : public SlotTy {
		FunTy fun;

		SlotImpl(Signal* signal, Object* context, const FunTy& fun) : SlotTy(signal, context), fun(fun) {
		}

		void invoke(ArgTys&&... args) override {
			fun(std::forward<ArgTys>(args)...);
		}
	};

	PodVector<SlotTy*> m_slots;
	int m_emitting = 0;
};

inline void disconnectMe() {
	g_disconnectMe = true;
}

} // namespace sgf

#include "object.h"

#endif
