Local<Value> Http2Session::Http2Settings::Pack() {
  const size_t len = count_ * 6;
  Local<Value> buf = Buffer::New(env(), len).ToLocalChecked();
  ssize_t ret =
      nghttp2_pack_settings_payload(
        reinterpret_cast<uint8_t*>(Buffer::Data(buf)), len,
        &entries_[0], count_);
  if (ret >= 0)
    return buf;
  else
    return Undefined(env()->isolate());
}