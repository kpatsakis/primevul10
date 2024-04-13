void Http2Session::HandleAltSvcFrame(const nghttp2_frame* frame) {
  if (!(js_fields_[kBitfield] & (1 << kSessionHasAltsvcListeners))) return;
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);

  int32_t id = GetFrameID(frame);

  nghttp2_extension ext = frame->ext;
  nghttp2_ext_altsvc* altsvc = static_cast<nghttp2_ext_altsvc*>(ext.payload);
  Debug(this, "handling altsvc frame");

  Local<Value> argv[3] = {
    Integer::New(isolate, id),
    String::NewFromOneByte(isolate,
                           altsvc->origin,
                           NewStringType::kNormal,
                           altsvc->origin_len).ToLocalChecked(),
    String::NewFromOneByte(isolate,
                           altsvc->field_value,
                           NewStringType::kNormal,
                           altsvc->field_value_len).ToLocalChecked(),
  };

  MakeCallback(env()->onaltsvc_string(), arraysize(argv), argv);
}