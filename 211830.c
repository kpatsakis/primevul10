inline StringData* StringData::Make(char* data, AttachStringMode) {
  SCOPE_EXIT { free(data); };
  return Make(data, CopyString);
}