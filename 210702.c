void BlobURLRequestJob::CloseStream() {
  if (stream_ != NULL) {
    stream_->Close();
    stream_.reset(NULL);
  }
}
