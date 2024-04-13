void BlobURLRequestJob::AdvanceItem() {
  CloseStream();

  item_index_++;
  current_item_offset_ = 0;
}
