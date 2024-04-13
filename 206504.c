void TLSWrap::NewSessionDoneCb() {
  Debug(this, "NewSessionDoneCb()");
  Cycle();
}