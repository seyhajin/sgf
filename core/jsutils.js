
mergeInto(LibraryManager.library, {

    sgfRegistry: {},

    sgfRegisterObject: function(obj, this_ptr) {
        obj._sgf_this_ptr = this_ptr;
        _sgfRegistry[this_ptr] = obj;
        return obj;
    },

    sgfGetObject: function(this_ptr) {
        return _sgfRegistry[this_ptr];
    },

    sgfDeregisterObject: function(this_ptr) {
        const obj = _sgfRegistry[this_ptr];
        delete _sgfRegistry[this_ptr];
        delete obj._sgf_this_ptr;
    },

});
