bool TLSWrap::IsClosing() {
  return underlying_stream()->IsClosing();
}