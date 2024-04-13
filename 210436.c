bool BlobURLRequestJob::GetMimeType(std::string* mime_type) const {
  if (!response_info_.get())
    return false;

  return response_info_->headers->GetMimeType(mime_type);
}
