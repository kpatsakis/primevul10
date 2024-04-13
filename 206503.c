int TLSWrap::GetFD() {
  return underlying_stream()->GetFD();
}