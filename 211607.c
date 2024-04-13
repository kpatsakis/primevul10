int MemFile::getc() {
  assertx(m_len != -1);
  return File::getc();
}