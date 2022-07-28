#pragma once

#include <core/core.hh>

namespace sgf {

class WebSocket : public Object{
public:
	SGF_OBJECT_TYPE(WebSocket, Object);

	Signal<> open;
	Signal<> closed;
	Signal<String> error;
	Signal<String> message;

	WebSocket(CString url);
	~WebSocket();

	void send(CString message);

	void close();

private:
	struct Impl;
	Impl* m_impl{};
};

} // namespace wb
