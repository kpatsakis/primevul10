void BlobURLRequestJob::AdvanceBytesRead(int result) {
  DCHECK_GT(result, 0);

  current_item_offset_ += result;
  if (current_item_offset_ == item_length_list_[item_index_])
    AdvanceItem();

  remaining_bytes_ -= result;
  DCHECK_GE(remaining_bytes_, 0);

  read_buf_offset_ += result;
  read_buf_remaining_bytes_ -= result;
  DCHECK_GE(read_buf_remaining_bytes_, 0);
}
