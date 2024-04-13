ShutdownWrap* ShutdownWrap::FromObject(v8::Local<v8::Object> req_wrap_obj) {
  return static_cast<ShutdownWrap*>(StreamReq::FromObject(req_wrap_obj));
}