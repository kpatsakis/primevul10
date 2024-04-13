void TLSWrap::OnStreamAfterWrite(WriteWrap* req_wrap, int status) {
  Debug(this, "OnStreamAfterWrite(status = %d)", status);
  if (current_empty_write_ != nullptr) {
    Debug(this, "Had empty write");
    WriteWrap* finishing = current_empty_write_;
    current_empty_write_ = nullptr;
    finishing->Done(status);
    return;
  }

  if (ssl_ == nullptr) {
    Debug(this, "ssl_ == nullptr, marking as cancelled");
    status = UV_ECANCELED;
  }

  // Handle error
  if (status) {
    if (shutdown_) {
      Debug(this, "Ignoring error after shutdown");
      return;
    }

    // Notify about error
    InvokeQueued(status);
    return;
  }

  // Commit
  crypto::NodeBIO::FromBIO(enc_out_)->Read(nullptr, write_size_);

  // Ensure that the progress will be made and `InvokeQueued` will be called.
  ClearIn();

  // Try writing more data
  write_size_ = 0;
  EncOut();
}