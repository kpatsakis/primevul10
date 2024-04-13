int BlobURLRequestJob::ComputeBytesToRead() const {
  int64 current_item_remaining_bytes =
      item_length_list_[item_index_] - current_item_offset_;
  int bytes_to_read = (read_buf_remaining_bytes_ > current_item_remaining_bytes)
      ? static_cast<int>(current_item_remaining_bytes)
      : read_buf_remaining_bytes_;
  if (bytes_to_read > remaining_bytes_)
    bytes_to_read = static_cast<int>(remaining_bytes_);
  return bytes_to_read;
}
