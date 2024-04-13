int TLSWrap::ReadStart() {
  Debug(this, "ReadStart()");
  if (stream_ != nullptr)
    return stream_->ReadStart();
  return 0;
}