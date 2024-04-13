int TLSWrap::ReadStop() {
  Debug(this, "ReadStop()");
  if (stream_ != nullptr)
    return stream_->ReadStop();
  return 0;
}