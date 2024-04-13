void utf16_to_utf8(UncheckedBuffer &buf, unsigned short utf16) {
  if (LIKELY(utf16 < 0x80)) {
    buf.append((char)utf16);
    return;
  }
  return utf16_to_utf8_tail(buf, utf16);
}