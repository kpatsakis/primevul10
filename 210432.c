void BlobURLRequestJob::DidOpen(base::PlatformFileError rv,
                                base::PassPlatformFile file,
                                bool created) {
  if (rv != base::PLATFORM_FILE_OK) {
    NotifyFailure(net::ERR_FAILED);
    return;
  }

  DCHECK(!stream_.get());
  stream_.reset(new net::FileStream(file.ReleaseValue(), kFileOpenFlags));

  const BlobData::Item& item = blob_data_->items().at(item_index_);
  {
    base::ThreadRestrictions::ScopedAllowIO allow_io;
    int64 offset = current_item_offset_ + static_cast<int64>(item.offset());
    if (offset > 0 && offset != stream_->Seek(net::FROM_BEGIN, offset)) {
      NotifyFailure(net::ERR_FAILED);
      return;
    }
  }

  ReadFile(item);
}
