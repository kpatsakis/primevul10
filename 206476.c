void Http2Session::HandlePingFrame(const nghttp2_frame* frame) {
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);
  Local<Value> arg;
  bool ack = frame->hd.flags & NGHTTP2_FLAG_ACK;
  if (ack) {
    Http2Ping* ping = PopPing();

    if (ping == nullptr) {
      // PING Ack is unsolicited. Treat as a connection error. The HTTP/2
      // spec does not require this, but there is no legitimate reason to
      // receive an unsolicited PING ack on a connection. Either the peer
      // is buggy or malicious, and we're not going to tolerate such
      // nonsense.
      arg = Integer::New(isolate, NGHTTP2_ERR_PROTO);
      MakeCallback(env()->error_string(), 1, &arg);
      return;
    }

    ping->Done(true, frame->ping.opaque_data);
    return;
  }

  if (!(js_fields_[kBitfield] & (1 << kSessionHasPingListeners))) return;
  // Notify the session that a ping occurred
  arg = Buffer::Copy(env(),
                      reinterpret_cast<const char*>(frame->ping.opaque_data),
                      8).ToLocalChecked();
  MakeCallback(env()->onping_string(), 1, &arg);
}