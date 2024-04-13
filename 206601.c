void StreamResource::EmitAfterShutdown(ShutdownWrap* w, int status) {
  DebugSealHandleScope seal_handle_scope;
  listener_->OnStreamAfterShutdown(w, status);
}