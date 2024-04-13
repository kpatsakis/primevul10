DataObjectItem* DataObjectItem::CreateFromHTML(const String& html,
                                               const KURL& base_url) {
  DataObjectItem* item =
      MakeGarbageCollected<DataObjectItem>(kStringKind, kMimeTypeTextHTML);
  item->data_ = html;
  item->base_url_ = base_url;
  return item;
}
