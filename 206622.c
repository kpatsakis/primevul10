ShutdownWrap::ShutdownWrap(
    StreamBase* stream,
    v8::Local<v8::Object> req_wrap_obj)
    : StreamReq(stream, req_wrap_obj) { }