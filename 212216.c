php_open_plain_file(const String& filename, const char *mode, FILE **fpp) {
  auto file = File::Open(filename, mode);
  auto plain_file = dyn_cast_or_null<PlainFile>(file);
  if (!plain_file) return nullptr;
  if (FILE* fp = plain_file->getStream()) {
  if (fpp) *fpp = fp;
    return file;
  }
  file->close();
  return nullptr;
}