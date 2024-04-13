inline bool HasHttp2Observer(Environment* env) {
  AliasedBuffer<uint32_t, Uint32Array>& observers =
      env->performance_state()->observers;
  return observers[performance::NODE_PERFORMANCE_ENTRY_TYPE_HTTP2] != 0;
}