void WriteWrap::OnDone(int status) {
  stream()->EmitAfterWrite(this, status);
  Dispose();
}