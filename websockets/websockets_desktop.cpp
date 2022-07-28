#include "websockets.h"

#include <IXWebSocket.h>

namespace sgf {

struct WebSocket::Impl {
	ix::WebSocket ixsocket;
};

WebSocket::WebSocket(CString url) : m_impl(new Impl) {

	m_impl->ixsocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
		postAppEvent(
			[this, type = msg->type, msg = msg->str, err = msg->errorInfo.reason] {
				switch (type) {
				case ix::WebSocketMessageType::Open:
					open.emit();
					break;
				case ix::WebSocketMessageType::Close:
					closed.emit();
					break;
				case ix::WebSocketMessageType::Message:
					message.emit(msg);
					break;
				case ix::WebSocketMessageType::Error:
					error.emit(err);
					break;
				default:
					break;
				}
			},
			this);
	});

	m_impl->ixsocket.setUrl(url);

	m_impl->ixsocket.start();
}

WebSocket::~WebSocket() {
	discardAppEvents(this);
	delete m_impl;
}

void WebSocket::send(CString msg) {
	m_impl->ixsocket.send(msg);
}

void WebSocket::close() {
	m_impl->ixsocket.close();
}

} // namespace sgf
