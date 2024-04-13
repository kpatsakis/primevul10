void Http2Stream::OnTrailers() {
  Debug(this, "let javascript know we are ready for trailers");
  CHECK(!this->IsDestroyed());
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);
  flags_ &= ~NGHTTP2_STREAM_FLAG_TRAILERS;
  MakeCallback(env()->ontrailers_string(), 0, nullptr);
}