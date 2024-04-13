int64_t MemFile::tell() {
  assertx(m_len != -1);
  return getPosition();
}