bool BlobURLRequestJob::ReadFile(const BlobData::Item& item) {
  DCHECK(stream_.get());
  DCHECK(stream_->IsOpen());
  DCHECK(read_buf_remaining_bytes_ >= bytes_to_read_);

  int rv = stream_->Read(read_buf_->data() + read_buf_offset_,
                         bytes_to_read_,
                         &io_callback_);

  if (rv == net::ERR_IO_PENDING) {
    SetStatus(net::URLRequestStatus(net::URLRequestStatus::IO_PENDING, 0));
    return false;
  }

  if (rv < 0) {
    NotifyFailure(net::ERR_FAILED);
    return false;
  }

  if (GetStatus().is_io_pending())
    DidRead(rv);
  else
    AdvanceBytesRead(rv);

  return true;
}
