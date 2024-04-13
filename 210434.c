void BlobURLRequestJob::DidResolve(base::PlatformFileError rv,
                                   const base::PlatformFileInfo& file_info) {
  if (rv == base::PLATFORM_FILE_ERROR_NOT_FOUND) {
    NotifyFailure(net::ERR_FILE_NOT_FOUND);
    return;
  } else if (rv != base::PLATFORM_FILE_OK) {
    NotifyFailure(net::ERR_FAILED);
    return;
  }

  const BlobData::Item& item = blob_data_->items().at(item_index_);
  DCHECK(item.type() == BlobData::TYPE_FILE);

  if (!item.expected_modification_time().is_null() &&
      item.expected_modification_time().ToTimeT() !=
          file_info.last_modified.ToTimeT()) {
    NotifyFailure(net::ERR_FILE_NOT_FOUND);
    return;
  }

  int64 item_length = static_cast<int64>(item.length());
  if (item_length == -1)
    item_length = file_info.size;

  item_length_list_.push_back(item_length);
  total_size_ += item_length;

  item_index_++;
  CountSize();
}
