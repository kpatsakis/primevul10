int string_crc32(const char *p, int len) {
  uint32_t crcinit = 0;
  register int32_t crc = crcinit ^ 0xFFFFFFFF;
  for (; len--; ++p) {
    crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32tab[(crc ^ (*p)) & 0xFF];
  }
  return crc ^ 0xFFFFFFFF;
}