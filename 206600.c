void StreamResource::EmitAfterWrite(WriteWrap* w, int status) {
  DebugSealHandleScope seal_handle_scope;
  listener_->OnStreamAfterWrite(w, status);
}