void BlobURLRequestJob::GetResponseInfo(net::HttpResponseInfo* info) {
  if (response_info_.get())
    *info = *response_info_;
}
