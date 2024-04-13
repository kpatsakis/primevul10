DataObjectItem* DataObjectItem::CreateFromSharedBuffer(
    scoped_refptr<SharedBuffer> buffer,
    const KURL& source_url,
    const String& filename_extension,
    const AtomicString& content_disposition) {
  DataObjectItem* item = MakeGarbageCollected<DataObjectItem>(
      kFileKind,
      MIMETypeRegistry::GetWellKnownMIMETypeForExtension(filename_extension));
  item->shared_buffer_ = std::move(buffer);
  item->filename_extension_ = filename_extension;
  item->title_ = content_disposition;
  item->base_url_ = source_url;
  return item;
}
