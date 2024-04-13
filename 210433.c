void BlobURLRequestJob::DidRead(int result) {
  if (result < 0) {
    NotifyFailure(net::ERR_FAILED);
    return;
  }
  SetStatus(net::URLRequestStatus());  // Clear the IO_PENDING status

  AdvanceBytesRead(result);

  if (!read_buf_remaining_bytes_) {
    int bytes_read = ReadCompleted();
    NotifyReadComplete(bytes_read);
    return;
  }

  int bytes_read = 0;
  if (ReadLoop(&bytes_read))
    NotifyReadComplete(bytes_read);
}
