  bool parseString() {
    int len;
    if (!parseRawString(&len)) return false;
    auto const start = reinterpret_cast<char*>(top);
    pushStringData(StringData::Make(start, len, CopyString));
    return true;
  }