void StreamBase::JSMethod(const FunctionCallbackInfo<Value>& args) {
  StreamBase* wrap = StreamBase::FromObject(args.Holder().As<Object>());
  if (wrap == nullptr) return;

  if (!wrap->IsAlive()) return args.GetReturnValue().Set(UV_EINVAL);

  AsyncHooks::DefaultTriggerAsyncIdScope trigger_scope(wrap->GetAsyncWrap());
  args.GetReturnValue().Set((wrap->*Method)(args));
}