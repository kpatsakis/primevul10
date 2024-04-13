inline strhash_t StringData::hash(const char* s, size_t len) {
  return hash_string_i(s, len);
}