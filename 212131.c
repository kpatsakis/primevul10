inline StringData* StringData::Make(const char* data, CopyStringMode) {
  return Make(data, strlen(data), CopyString);
}