void MemFile::unzip() {
  assertx(m_len != -1);
  assertx(!m_malloced);
  assertx(m_cursor == 0);
  int len = m_len;
  char *data = gzdecode(m_data, len);
  if (data == nullptr) {
    raise_fatal_error((std::string("cannot unzip mem stream: ") +
                               getName()).c_str());
  }
  m_data = data;
  m_malloced = true;
  m_len = len;
}