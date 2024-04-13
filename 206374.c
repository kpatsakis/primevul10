void TLSWrap::OnClientHelloParseEnd(void* arg) {
  TLSWrap* c = static_cast<TLSWrap*>(arg);
  Debug(c, "OnClientHelloParseEnd()");
  c->Cycle();
}