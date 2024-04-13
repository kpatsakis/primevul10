void StreamBase::GetBytesWritten(const FunctionCallbackInfo<Value>& args) {
  StreamBase* wrap = StreamBase::FromObject(args.This().As<Object>());
  if (wrap == nullptr) return args.GetReturnValue().Set(0);

  // uint64_t -> double. 53bits is enough for all real cases.
  args.GetReturnValue().Set(static_cast<double>(wrap->bytes_written_));
}