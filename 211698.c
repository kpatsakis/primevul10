bool MemFile::eof() {
  assertx(m_len != -1);
  int64_t avail = bufferedLen();
  if (avail > 0) {
    return false;
  }
  return m_cursor == m_len;
}