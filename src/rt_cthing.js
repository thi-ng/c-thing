// emscripten JS library support

mergeInto(LibraryManager.library, {
    ct_js_test_call : function() {
        return 23;
    }
});
