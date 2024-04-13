std::unique_ptr<ClipboardReader> ClipboardReader::Create(
    const String& mime_type) {
  if (mime_type == kMimeTypeImagePng)
    return std::make_unique<ClipboardImageReader>();
  if (mime_type == kMimeTypeTextPlain)
    return std::make_unique<ClipboardTextReader>();

  return nullptr;
}
