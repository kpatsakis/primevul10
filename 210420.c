DataObjectItem* DataObjectItem::CreateFromString(const String& type,
                                                 const String& data) {
  DataObjectItem* item =
      MakeGarbageCollected<DataObjectItem>(kStringKind, type);
  item->data_ = data;
  return item;
}
