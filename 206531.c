ssize_t Http2Session::OnCallbackPadding(size_t frameLen,
                                        size_t maxPayloadLen) {
  if (frameLen == 0) return 0;
  Debug(this, "using callback to determine padding");
  Isolate* isolate = env()->isolate();
  HandleScope handle_scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);

  AliasedBuffer<uint32_t, Uint32Array>& buffer =
      env()->http2_state()->padding_buffer;
  buffer[PADDING_BUF_FRAME_LENGTH] = frameLen;
  buffer[PADDING_BUF_MAX_PAYLOAD_LENGTH] = maxPayloadLen;
  buffer[PADDING_BUF_RETURN_VALUE] = frameLen;
  MakeCallback(env()->ongetpadding_string(), 0, nullptr);
  uint32_t retval = buffer[PADDING_BUF_RETURN_VALUE];
  retval = std::min(retval, static_cast<uint32_t>(maxPayloadLen));
  retval = std::max(retval, static_cast<uint32_t>(frameLen));
  Debug(this, "using padding size %d", retval);
  return retval;
}