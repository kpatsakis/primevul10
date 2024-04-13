inline void StreamResource::EmitWantsWrite(size_t suggested_size) {
#ifdef DEBUG
  v8::SealHandleScope handle_scope(v8::Isolate::GetCurrent());
#endif
  listener_->OnStreamWantsWrite(suggested_size);
}