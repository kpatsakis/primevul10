inline void StreamResource::EmitAfterWrite(WriteWrap* w, int status) {
#ifdef DEBUG
  v8::SealHandleScope handle_scope(v8::Isolate::GetCurrent());
#endif
  listener_->OnStreamAfterWrite(w, status);
}