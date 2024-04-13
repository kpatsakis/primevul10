Http2Priority::Http2Priority(Environment* env,
                             Local<Value> parent,
                             Local<Value> weight,
                             Local<Value> exclusive) {
  Local<Context> context = env->context();
  int32_t parent_ = parent->Int32Value(context).ToChecked();
  int32_t weight_ = weight->Int32Value(context).ToChecked();
  bool exclusive_ = exclusive->BooleanValue(context).ToChecked();
  Debug(env, DebugCategory::HTTP2STREAM,
        "Http2Priority: parent: %d, weight: %d, exclusive: %d\n",
        parent_, weight_, exclusive_);
  nghttp2_priority_spec_init(&spec, parent_, weight_, exclusive_ ? 1 : 0);
}