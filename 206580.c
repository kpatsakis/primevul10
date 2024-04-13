void StreamResource::EmitWantsWrite(size_t suggested_size) {
  DebugSealHandleScope seal_handle_scope;
  listener_->OnStreamWantsWrite(suggested_size);
}