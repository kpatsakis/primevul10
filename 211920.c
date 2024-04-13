bool MemFile::rewind() {
  assertx(m_len != -1);
  m_cursor = 0;
  setWritePosition(0);
  setReadPosition(0);
  setPosition(0);
  return true;
}