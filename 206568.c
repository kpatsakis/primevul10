void StreamBase::SetWriteResult(const StreamWriteResult& res) {
  env_->stream_base_state()[kBytesWritten] = res.bytes;
  env_->stream_base_state()[kLastWriteWasAsync] = res.async;
}