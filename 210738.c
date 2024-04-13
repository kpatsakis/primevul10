bool VariableUnserializer::matchString(folly::StringPiece str) {
  const char* p = m_buf;
  assertx(p <= m_end);
  int total = 0;
  if (*p == 'S') {
    total = 2 + 8 + 1;
    if (p + total > m_end) return false;
    p++;
    if (*p++ != ':') return false;
    auto const sd = *reinterpret_cast<StringData*const*>(p);
    assertx(sd->isStatic());
    if (str.compare(sd->slice()) != 0) return false;
    p += size_t(8);
  } else {
    const auto ss = str.size();
    if (ss >= 100) return false;
    int digits = ss >= 10 ? 2 : 1;
    total = 2 + digits + 2 + ss + 2;
    if (p + total > m_end) return false;
    if (*p++ != 's') return false;
    if (*p++ != ':') return false;
    if (digits == 2) {
      if (*p++ != '0' + ss/10) return false;
      if (*p++ != '0' + ss%10) return false;
    } else {
      if (*p++ != '0' + ss) return false;
    }
    if (*p++ != ':') return false;
    if (*p++ != '\"') return false;
    if (memcmp(p, str.data(), ss)) return false;
    p += ss;
    if (*p++ != '\"') return false;
  }
  if (*p++ != ';') return false;
  assertx(m_buf + total == p);
  m_buf = p;
  return true;
}