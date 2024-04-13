bool strtr_slow(const Array& arr, StringBuffer& result, String& key,
                const char*s, int& pos, int minlen, int maxlen) {

  memcpy(key.mutableData(), s + pos, maxlen);
  for (int len = maxlen; len >= minlen; len--) {
    key.setSize(len);
    auto const tv = arr->get(key);
    if (tv.is_init()) {
      String replace = tvCastToString(tv);
      if (!replace.empty()) {
        result.append(replace);
      }
      pos += len;
      return true;
    }
  }
  return false;
}