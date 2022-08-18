#pragma once

#include "function.h"
#include "std.h"

#include <chrono>
#include <cstdio>
#include <sstream>

namespace sgf {

using DebugOutputFunc = Function<void(CString)>;

extern thread_local DebugOutputFunc debugOutputFunc;

void defaultDebugOutputFunc(CString);

String debugTimestamp();

class DebugStream {

	using time_point = std::chrono::time_point<std::chrono::system_clock>;

	struct Rep {
		time_point time;
		const char* file;
		int line;
		std::stringstream buf;

		Rep(time_point time, const char*file, int line) : time(time),file(file),line(line){}
	};

	Rep* m_rep;

	template <class ValueTy> friend DebugStream&& operator<<(DebugStream&& dbgstream, const ValueTy& value) {
		auto rep = dbgstream.m_rep;
		if (rep->buf.tellp()) rep->buf << ' ';
		rep->buf << value;
		return std::move(dbgstream);
	}

public:
	DebugStream(const char* file, int line);

	~DebugStream();

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

#define debug() DebugStream(__FILE__, __LINE__)

} // namespace sgf
