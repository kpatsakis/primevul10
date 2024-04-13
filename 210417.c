DataObjectItem* DataObjectItem::CreateFromFile(File* file) {
  DataObjectItem* item =
      MakeGarbageCollected<DataObjectItem>(kFileKind, file->type());
  item->file_ = file;
  return item;
}
