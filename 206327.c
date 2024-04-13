void JSStream::ReadBuffer(const FunctionCallbackInfo<Value>& args) {
  JSStream* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  CHECK(Buffer::HasInstance(args[0]));
  char* data = Buffer::Data(args[0]);
  int len = Buffer::Length(args[0]);

  // Repeatedly ask the stream's owner for memory, copy the data that we
  // just read from JS into those buffers and emit them as reads.
  while (len != 0) {
    uv_buf_t buf = wrap->EmitAlloc(len);
    ssize_t avail = len;
    if (static_cast<ssize_t>(buf.len) < avail)
      avail = buf.len;

    memcpy(buf.base, data, avail);
    data += avail;
    len -= avail;
    wrap->EmitRead(avail, buf);
  }
}