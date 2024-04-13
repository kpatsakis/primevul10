  bool parseMixedKey() {
    int len;
    int64_t num;
    if (!parseRawString(&len)) return false;
    auto const start = reinterpret_cast<char*>(top);
    auto const slice = folly::StringPiece(start, len);
    start[len] = '\0';
    if (container_type != JSONContainerType::HACK_ARRAYS &&
        container_type != JSONContainerType::LEGACY_HACK_ARRAYS &&
        is_strictly_integer(start, len, num)) {
      pushInt64(num);
    } else if (auto const str = lookupStaticString(slice)) {
      auto const tv = top++;
      tv->m_type = KindOfPersistentString;
      tv->m_data.pstr = str;
    } else {
      pushStringData(StringData::Make(start, len, CopyString));
    }
    return true;
  }