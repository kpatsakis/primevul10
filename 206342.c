int Http2Session::OnStreamClose(nghttp2_session* handle,
                                int32_t id,
                                uint32_t code,
                                void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);
  Environment* env = session->env();
  Isolate* isolate = env->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env->context();
  Context::Scope context_scope(context);
  Debug(session, "stream %d closed with code: %d", id, code);
  Http2Stream* stream = session->FindStream(id);
  // Intentionally ignore the callback if the stream does not exist or has
  // already been destroyed
  if (stream == nullptr || stream->IsDestroyed())
    return 0;

  stream->Close(code);
  // It is possible for the stream close to occur before the stream is
  // ever passed on to the javascript side. If that happens, skip straight
  // to destroying the stream. We can check this by looking for the
  // onstreamclose function. If it exists, then the stream has already
  // been passed on to javascript.
  Local<Value> fn =
      stream->object()->Get(context, env->onstreamclose_string())
          .ToLocalChecked();

  if (!fn->IsFunction()) {
    stream->Destroy();
    return 0;
  }

  Local<Value> arg = Integer::NewFromUnsigned(isolate, code);
  stream->MakeCallback(fn.As<Function>(), 1, &arg);
  return 0;
}