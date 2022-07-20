
mergeInto(LibraryManager.library, {

    sgfRegistry: {},

    sgfRegisterObject: function(obj, this_ptr) {
        _sgfRegistry[this_ptr] = obj;
        obj._sgf_this_ptr = this_ptr;
        return obj;
    },

    sgfGetObject: function(this_ptr) {
        return _sgfRegistry[this_ptr];
    },

    sgfDeregisterObject: function(obj_or_this) {
        const this_ptr = (typeof obj_or_this === "object") ? obj_or_this._sgf_this_ptr : obj_or_this;
        delete _sgfRegistry[this_ptr];
    }

});
