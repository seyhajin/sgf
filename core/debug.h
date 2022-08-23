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
public:
	using Emit = Function<void(CString)>;

	DebugStream(Emit emit);

	~DebugStream();

	DebugStream(DebugStream&& rhs) noexcept : m_rep(rhs.m_rep) {
		rhs.m_rep = nullptr;
	}

	DebugStream(const DebugStream&) = delete;

	DebugStream& operator=(const DebugStream&) = delete;

	DebugStream& operator=(DebugStream&& that) = delete;

protected:
	struct Rep {
		using clock = std::chrono::system_clock;
		using time_point = std::chrono::time_point<clock>;

		std::stringstream buf;
		Emit emit;

		Rep(Emit emit) : emit(std::move(emit)) {
			buf.setf(std::ios::fixed);
			buf.precision(4);
		}
	};

	Rep* m_rep;

	template <class ValueTy> friend DebugStream&& operator<<(DebugStream&& strstream, const ValueTy& value) {
		auto rep = strstream.m_rep;
		if (rep->buf.tellp()) rep->buf << ' ';
		rep->buf << value;
		return std::move(strstream);
	}
};

DebugStream debug(const char* file = nullptr, int line = -1);

} // namespace sgf
