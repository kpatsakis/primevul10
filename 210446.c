void BlobURLRequestJob::Seek(int64 offset) {
  for (item_index_ = 0;
       item_index_ < blob_data_->items().size() &&
           offset >= item_length_list_[item_index_];
       ++item_index_) {
    offset -= item_length_list_[item_index_];
  }

  current_item_offset_ = offset;
}
