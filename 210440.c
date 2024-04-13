 void BlobURLRequestJob::NotifySuccess() {
  int status_code = 0;
  std::string status_text;
  if (byte_range_set_ && byte_range_.IsValid()) {
    status_code = kHTTPPartialContent;
    status_text += kHTTPPartialContentText;
  } else {
    status_code = kHTTPOk;
    status_text = kHTTPOKText;
  }
  HeadersCompleted(status_code, status_text);
}
