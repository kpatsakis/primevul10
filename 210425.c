bool DataObjectItem::HasFileSystemId() const {
  return kind_ == kFileKind && !file_system_id_.IsEmpty();
}
