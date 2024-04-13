bool BlobURLRequestJob::ReadBytes(const BlobData::Item& item) {
  DCHECK(read_buf_remaining_bytes_ >= bytes_to_read_);

  memcpy(read_buf_->data() + read_buf_offset_,
         &item.data().at(0) + item.offset() + current_item_offset_,
         bytes_to_read_);

  AdvanceBytesRead(bytes_to_read_);
  return true;
}
