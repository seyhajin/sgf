#pragma once

#include "function.h"
#include "std.h"

#include <cstdio>
#include <sstream>

namespace sgf {

using DebugOutputFunc = Function<void(CString)>;

extern thread_local DebugOutputFunc debugOutputFunc;

void defaultDebugOutputFunc(CString);

String debugTimestamp();

class DebugStream {

	struct Rep {
		std::stringstream buf;
	};

	Rep* m_rep = new Rep;

	template <class ValueTy> friend DebugStream&& operator<<(DebugStream&& dbgstream, const ValueTy& value) {
		auto rep = dbgstream.m_rep;
		if (rep->buf.tellp()) rep->buf << ' ';
		rep->buf << value;
		return std::move(dbgstream);
	}

public:
	DebugStream() = default;

	~DebugStream() {
		debugOutputFunc(debugTimestamp() + " " + m_rep->buf.str());
		delete m_rep;
	}

	DebugStream(const DebugStream&) = delete;

	DebugStream(DebugStream&& rhs) noexcept : m_rep(rhs.m_rep) {
		rhs.m_rep = nullptr;
	}

	DebugStream& operator=(const DebugStream& rhs) = delete;

	DebugStream& operator=(DebugStream&& rhs) noexcept {
		m_rep = rhs.m_rep;
		rhs.m_rep = nullptr;
		return *this;
	}
};

inline DebugStream debug() {
	return {};
}

} // namespace sgf
