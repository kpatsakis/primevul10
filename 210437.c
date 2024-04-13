int BlobURLRequestJob::GetResponseCode() const {
  if (!response_info_.get())
    return -1;

  return response_info_->headers->response_code();
}
