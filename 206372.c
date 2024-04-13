LibuvStreamWrap* LibuvStreamWrap::From(Environment* env, Local<Object> object) {
  Local<FunctionTemplate> sw = env->libuv_stream_wrap_ctor_template();
  CHECK(!sw.IsEmpty() && sw->HasInstance(object));
  return Unwrap<LibuvStreamWrap>(object);
}