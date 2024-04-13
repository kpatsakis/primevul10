int StreamBase::UseUserBuffer(const FunctionCallbackInfo<Value>& args) {
  CHECK(Buffer::HasInstance(args[0]));

  uv_buf_t buf = uv_buf_init(Buffer::Data(args[0]), Buffer::Length(args[0]));
  PushStreamListener(new CustomBufferJSListener(buf));
  return 0;
}