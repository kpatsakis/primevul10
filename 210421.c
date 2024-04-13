DataObjectItem* DataObjectItem::CreateFromURL(const String& url,
                                              const String& title) {
  DataObjectItem* item =
      MakeGarbageCollected<DataObjectItem>(kStringKind, kMimeTypeTextURIList);
  item->data_ = url;
  item->title_ = title;
  return item;
}
