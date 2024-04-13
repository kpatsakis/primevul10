void LibuvStreamWrap::GetWriteQueueSize(
    const FunctionCallbackInfo<Value>& info) {
  LibuvStreamWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, info.This());

  if (wrap->stream() == nullptr) {
    info.GetReturnValue().Set(0);
    return;
  }

  uint32_t write_queue_size = wrap->stream()->write_queue_size;
  info.GetReturnValue().Set(write_queue_size);
}