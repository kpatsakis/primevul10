bool BlobURLRequestJob::DispatchReadFile(const BlobData::Item& item) {
  if (stream_ != NULL)
    return ReadFile(item);

  base::FileUtilProxy::CreateOrOpen(
      file_thread_proxy_, item.file_path(), kFileOpenFlags,
      callback_factory_.NewCallback(&BlobURLRequestJob::DidOpen));
  SetStatus(net::URLRequestStatus(net::URLRequestStatus::IO_PENDING, 0));
  return false;
}
