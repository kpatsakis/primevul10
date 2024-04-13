DataObjectItem* DataObjectItem::CreateFromFileWithFileSystemId(
    File* file,
    const String& file_system_id) {
  DataObjectItem* item =
      MakeGarbageCollected<DataObjectItem>(kFileKind, file->type());
  item->file_ = file;
  item->file_system_id_ = file_system_id;
  return item;
}
