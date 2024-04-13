void StreamBase::GetExternal(const FunctionCallbackInfo<Value>& args) {
  StreamBase* wrap = StreamBase::FromObject(args.This().As<Object>());
  if (wrap == nullptr) return;

  Local<External> ext = External::New(args.GetIsolate(), wrap);
  args.GetReturnValue().Set(ext);
}