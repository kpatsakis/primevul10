StreamReq::StreamReq(
    StreamBase* stream,
    v8::Local<v8::Object> req_wrap_obj) : stream_(stream) {
  AttachToObject(req_wrap_obj);
}