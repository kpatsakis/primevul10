MaybeLocal<Value> StreamBase::CallJSOnreadMethod(ssize_t nread,
                                                 Local<ArrayBuffer> ab,
                                                 size_t offset,
                                                 StreamBaseJSChecks checks) {
  Environment* env = env_;

  DCHECK_EQ(static_cast<int32_t>(nread), nread);
  DCHECK_LE(offset, INT32_MAX);

  if (checks == DONT_SKIP_NREAD_CHECKS) {
    if (ab.IsEmpty()) {
      DCHECK_EQ(offset, 0);
      DCHECK_LE(nread, 0);
    } else {
      DCHECK_GE(nread, 0);
    }
  }

  env->stream_base_state()[kReadBytesOrError] = nread;
  env->stream_base_state()[kArrayBufferOffset] = offset;

  Local<Value> argv[] = {
    ab.IsEmpty() ? Undefined(env->isolate()).As<Value>() : ab.As<Value>()
  };

  AsyncWrap* wrap = GetAsyncWrap();
  CHECK_NOT_NULL(wrap);
  Local<Value> onread = wrap->object()->GetInternalField(
      StreamBase::kOnReadFunctionField);
  CHECK(onread->IsFunction());
  return wrap->MakeCallback(onread.As<Function>(), arraysize(argv), argv);
}