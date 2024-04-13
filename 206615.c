void StreamBase::GetFD(const FunctionCallbackInfo<Value>& args) {
  // Mimic implementation of StreamBase::GetFD() and UDPWrap::GetFD().
  StreamBase* wrap = StreamBase::FromObject(args.This().As<Object>());
  if (wrap == nullptr) return args.GetReturnValue().Set(UV_EINVAL);

  if (!wrap->IsAlive()) return args.GetReturnValue().Set(UV_EINVAL);

  args.GetReturnValue().Set(wrap->GetFD());
}