WriteWrap* WriteWrap::FromObject(v8::Local<v8::Object> req_wrap_obj) {
  return static_cast<WriteWrap*>(StreamReq::FromObject(req_wrap_obj));
}