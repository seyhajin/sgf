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
		bool space = false;
	};

	Rep* m_rep = new Rep;

	[[noreturn]] friend void fail(DebugStream&& dbgstream, const char* file, int line) {
		auto str = dbgstream.m_rep->buf.str();
		dbgstream.m_rep->buf.str({});
		fail(str, file, line);
	}

	template <class T> friend DebugStream&& operator<<(DebugStream&& dbgstream, const T& value) {
		auto rep = dbgstream.m_rep;
		if (rep->space) rep->buf << ' ';
		rep->space = true;
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
