TypedValue HHVM_FUNCTION(chunk_split, const String& body,
                      int chunklen, const String& end) {
  return tvReturn(StringUtil::ChunkSplit(body, chunklen, end));
}