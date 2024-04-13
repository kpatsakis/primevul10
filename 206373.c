inline void StreamResource::EmitAfterShutdown(ShutdownWrap* w, int status) {
#ifdef DEBUG
  v8::SealHandleScope handle_scope(v8::Isolate::GetCurrent());
#endif
  listener_->OnStreamAfterShutdown(w, status);
}