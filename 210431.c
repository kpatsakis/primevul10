void BlobURLRequestJob::CountSize() {
  for (; item_index_ < blob_data_->items().size(); ++item_index_) {
    const BlobData::Item& item = blob_data_->items().at(item_index_);
    int64 item_length = static_cast<int64>(item.length());

    if (item.type() == BlobData::TYPE_FILE) {
      ResolveFile(item.file_path());
      return;
    }

    item_length_list_.push_back(item_length);
    total_size_ += item_length;
  }

  item_index_ = 0;

  if (!byte_range_.ComputeBounds(total_size_)) {
    NotifyFailure(net::ERR_REQUEST_RANGE_NOT_SATISFIABLE);
    return;
  }

  remaining_bytes_ = byte_range_.last_byte_position() -
                     byte_range_.first_byte_position() + 1;
  DCHECK_GE(remaining_bytes_, 0);

  if (byte_range_.first_byte_position())
    Seek(byte_range_.first_byte_position());

  NotifySuccess();
}
