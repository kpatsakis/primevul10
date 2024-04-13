String stringForEachBuffered(uint32_t bufLen, const String& str, Op action) {
  StringBuffer sb(bufLen);
  auto sl = str.slice();
  const char* src = sl.begin();
  const char* end = sl.end();

  for (; src < end; ++src) {
    action(sb, src, end);
  }

  return sb.detach();
}