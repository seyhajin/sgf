#include "websockets.h"

namespace sgf {

// ***** WebSocket constructor *****

EM_CALLBACK void sgfWSOnOpen(WebSocket* ws_ptr) {
	ws_ptr->open.emit();
}

EM_CALLBACK void sgfWSOnClose(WebSocket* ws_ptr) {
	ws_ptr->closed.emit();
}

EM_CALLBACK void sgfWSOnError(WebSocket* ws_ptr, const char* err_cstr) {
	ws_ptr->error.emit(String(err_cstr));
}

EM_CALLBACK void sgfWSOnMessage(WebSocket* ws_ptr, const char* msg_cstr) {
	ws_ptr->message.emit(String(msg_cstr));
}

EM_EXTERN void sgfWSCreate(WebSocket* ws_ptr, const char* url_cstr);

WebSocket::WebSocket(CString url) { //
	sgfWSCreate(this, url.c_str());
}

// ***** WebSocket destructor *****

EM_EXTERN void sgfWSDestroy(WebSocket* ws_ptr);

WebSocket::~WebSocket() { //
	sgfWSDestroy(this);
}

// ***** WebSocket::send *****

EM_EXTERN void sgfWSSend(WebSocket* ws_ptr, const char* msg_cstr);

void WebSocket::send(CString msg) { //
	sgfWSSend(this, msg.c_str());
}

// ***** WebSocket::close *****

EM_EXTERN void sgfWSClose(WebSocket* ws_ptr);

void WebSocket::close() { //
	sgfWSClose(this);
}

} // namespace sgf
