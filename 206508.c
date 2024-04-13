inline void StreamReq::Done(int status, const char* error_str) {
  AsyncWrap* async_wrap = GetAsyncWrap();
  Environment* env = async_wrap->env();
  if (error_str != nullptr) {
    async_wrap->object()->Set(env->error_string(),
                              OneByteString(env->isolate(), error_str));
  }

  OnDone(status);
}