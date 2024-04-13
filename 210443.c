bool BlobURLRequestJob::ReadItem() {
  if (remaining_bytes_ == 0)
    return true;

  if (item_index_ >= blob_data_->items().size()) {
    NotifyFailure(net::ERR_FAILED);
    return false;
  }

  bytes_to_read_ = ComputeBytesToRead();

  if (bytes_to_read_ == 0) {
    AdvanceItem();
    return ReadItem();
  }

  const BlobData::Item& item = blob_data_->items().at(item_index_);
  switch (item.type()) {
    case BlobData::TYPE_DATA:
      return ReadBytes(item);
    case BlobData::TYPE_FILE:
      return DispatchReadFile(item);
    default:
      DCHECK(false);
      return false;
  }
}
