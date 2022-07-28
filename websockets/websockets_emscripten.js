mergeInto(LibraryManager.library, {

    sgfWSCreate: function (ws_ptr, url_cstr) {

        let ws;

        try {
            ws = new WebSocket(UTF8ToString(url_cstr));
            _sgfRegisterObject(ws, ws_ptr);
        } catch (error) {
            console.log("Exception creating WebSocket", error);
            return;
        }

        ws.addEventListener('open', (event) => {
            _sgfWSOnOpen(ws_ptr);
        });

        ws.addEventListener('close', (event) => {
            _sgfWSOnClose(ws_ptr);
        });

        ws.addEventListener('error', (event) => {
            const error = JSON.stringify(event, Object.getOwnPropertyNames(event));
            const error_cstr = allocateUTF8(error);
            _sgfWSOnError(ws_ptr, error_cstr);
            _free(error_cstr);
        });

        ws.addEventListener('message', (event) => {
            const msg_cstr = allocateUTF8(event.data);
            _sgfWSOnMessage(ws_ptr, msg_cstr);
            _free(msg_cstr);
        });
    },

    sgfWSDestroy: function (ws_ptr) {

        const ws = _sgfGetObject(ws_ptr);
        _sgfDeregisterObject(ws_ptr);

        ws.close();
    },

    sgfWSSend: function (ws_ptr, msg_cstr) {

        const ws = _sgfGetObject(ws_ptr);

        ws.send(UTF8ToString(msg_cstr));
    },

    sgwWSClose: function (ws_ptr) {

        const ws = _sgfGetObject(ws_ptr);

        ws.close();
    }

});
