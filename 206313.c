int Http2Session::OnInvalidFrame(nghttp2_session* handle,
                                 const nghttp2_frame* frame,
                                 int lib_error_code,
                                 void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);

  Debug(session, "invalid frame received, code: %d", lib_error_code);
  if (session->invalid_frame_count_++ > 1000 &&
      !IsReverted(SECURITY_REVERT_CVE_2019_9514)) {
    return 1;
  }

  // If the error is fatal or if error code is ERR_STREAM_CLOSED... emit error
  if (nghttp2_is_fatal(lib_error_code) ||
      lib_error_code == NGHTTP2_ERR_STREAM_CLOSED) {
    Environment* env = session->env();
    Isolate* isolate = env->isolate();
    HandleScope scope(isolate);
    Local<Context> context = env->context();
    Context::Scope context_scope(context);
    Local<Value> arg = Integer::New(isolate, lib_error_code);
    session->MakeCallback(env->error_string(), 1, &arg);
  }
  return 0;
}