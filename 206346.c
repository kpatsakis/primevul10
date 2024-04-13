void Http2Session::HandleOriginFrame(const nghttp2_frame* frame) {
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);

  Debug(this, "handling origin frame");

  nghttp2_extension ext = frame->ext;
  nghttp2_ext_origin* origin = static_cast<nghttp2_ext_origin*>(ext.payload);

  Local<Value> holder = Array::New(isolate);
  Local<Function> fn = env()->push_values_to_array_function();
  Local<Value> argv[NODE_PUSH_VAL_TO_ARRAY_MAX];

  size_t n = 0;
  while (n < origin->nov) {
    size_t j = 0;
    while (n < origin->nov && j < arraysize(argv)) {
      auto entry = origin->ov[n++];
      argv[j++] =
        String::NewFromOneByte(isolate,
                               entry.origin,
                               NewStringType::kNormal,
                               entry.origin_len).ToLocalChecked();
    }
    if (j > 0)
      fn->Call(context, holder, j, argv).ToLocalChecked();
  }

  MakeCallback(env()->onorigin_string(), 1, &holder);
}