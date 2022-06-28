#pragma once

#include "object.h"
#include "signal.h"

namespace sgf {

// ***** Signal ******
//
template <class... ArgTys> Signal<ArgTys...>::~Signal() {

	// TODO: think about this!
	if (m_emitting) panic("OOPS: Signal destroyed while emitting");

	while (!m_slots.empty()) {
		auto slot = m_slots.back();
		m_slots.pop_back();
		delete slot;
	}
}

// ***** Slot *****
//
template <class... ArgTys> Slot<ArgTys...>::Slot(Signal<ArgTys...>* signal, Object* context) : context(context) {

	if (!context) return;

	deleter = context->deleted.connect([this, signal] {
		deleter = nullptr;
		signal->disconnect(this);
	});
}

template <class... ArgTys> Slot<ArgTys...>::~Slot() {

	if (deleter) context->deleted.disconnect(deleter);
}

} // namespace sgf
