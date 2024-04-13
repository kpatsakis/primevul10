void Http2Session::Http2Ping::Done(bool ack, const uint8_t* payload) {
  session_->statistics_.ping_rtt = uv_hrtime() - startTime_;
  double duration = session_->statistics_.ping_rtt / 1e6;

  Local<Value> buf = Undefined(env()->isolate());
  if (payload != nullptr) {
    buf = Buffer::Copy(env()->isolate(),
                       reinterpret_cast<const char*>(payload),
                       8).ToLocalChecked();
  }

  Local<Value> argv[3] = {
    Boolean::New(env()->isolate(), ack),
    Number::New(env()->isolate(), duration),
    buf
  };
  MakeCallback(env()->ondone_string(), arraysize(argv), argv);
  delete this;
}