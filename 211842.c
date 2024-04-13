inline strhash_t StringData::hash_unsafe(const char* s, size_t len) {
  return hash_string_i_unsafe(s, len);
}