inline void ShutdownWrap::OnDone(int status) {
  stream()->EmitAfterShutdown(this, status);
  Dispose();
}