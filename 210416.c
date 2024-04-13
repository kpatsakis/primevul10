DataObjectItem* DataObjectItem::CreateFromClipboard(const String& type,
                                                    uint64_t sequence_number) {
  if (type == kMimeTypeImagePng) {
    return MakeGarbageCollected<DataObjectItem>(kFileKind, type,
                                                sequence_number);
  }
  return MakeGarbageCollected<DataObjectItem>(kStringKind, type,
                                              sequence_number);
}
