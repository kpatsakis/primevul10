int StreamBase::Shutdown(const FunctionCallbackInfo<Value>& args) {
  CHECK(args[0]->IsObject());
  Local<Object> req_wrap_obj = args[0].As<Object>();

  return Shutdown(req_wrap_obj);
}