inline void StreamReq::Dispose() {
  object()->SetAlignedPointerInInternalField(kStreamReqField, nullptr);
  delete this;
}