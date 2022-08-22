#include <websockets/websockets.hh>

using namespace sgf;

int main() {

	auto ws = new WebSocket("wss://websocket-echo.com");

	debug() << "Connecting...";

	ws->open.connect([ws] {
		debug() << "Open.";

		ws->message.connect([](CString msg) { debug() << "Received:" << msg; });

		ws->send("Hello there!");

		ws->send("Goodbye now!");
	});

	runAppEventLoop();
}
