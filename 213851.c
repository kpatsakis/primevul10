void Encode(const UnicodeEncoding encoding, const icu::UnicodeString& in,
            tstring* out) {
  if (encoding == UnicodeEncoding::UTF8) {
    out->clear();
    in.toUTF8String(*out);
  } else if (encoding == UnicodeEncoding::UTF16BE) {
    // TODO(gbillock): consider using the
    // extract(char *dest, int32_t destCapacity, UConverter *cnv)
    // for UTF16/32
    out->clear();  // subtle: must come before reserve()
    out->reserve(2 * in.length() + 1);
    const char16_t* buf = in.getBuffer();
    for (int i = 0; i < in.length(); ++i) {
      // Emit big-endian encoding for UTF-16 always.
      out->push_back((buf[i] & 0xFF00) >> 8);
      out->push_back(buf[i] & 0x00FF);
    }
  } else if (encoding == UnicodeEncoding::UTF32BE) {
    out->clear();  // subtle: must come before reserve()
    out->reserve(4 * in.countChar32() + 1);
    icu::StringCharacterIterator it(in);
    UChar32 ch;
    while (it.hasNext()) {
      ch = it.next32PostInc();
      out->push_back((ch & 0xFF000000) >> 24);
      out->push_back((ch & 0x00FF0000) >> 16);
      out->push_back((ch & 0x0000FF00) >> 8);
      out->push_back((ch & 0x000000FF));
    }
  }
}