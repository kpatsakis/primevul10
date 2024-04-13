Variant strtr_fast(const String& str, const Array& arr,
                   int minlen, int maxlen) {
  using PatternMask = uint64_t[256];
  auto mask = req::calloc_raw_array<PatternMask>(maxlen);
  SCOPE_EXIT { req::free(mask); };

  int pattern_id = 0;
  for (ArrayIter iter(arr); iter; ++iter, pattern_id++) {
    auto const search = iter.first().toString();
    auto slice = search.slice();

    for (auto i = 0; i < slice.size(); i++) {
      mask[i][(unsigned char)slice.data()[i]] |= (1LL << pattern_id);
    }
  }
  auto s = str.data();
  auto slen = str.size();
  StringBuffer result(slen);
  String key(maxlen, ReserveString);

  for (auto i = 0; i < slen;) {
    if ((i + maxlen) > slen) {
      maxlen = slen - i;
    }
    uint64_t match = ~0x0ULL;
    bool possible_match = false;

    for (auto pos = 0; pos < maxlen; pos++) {
      match &= mask[pos][(unsigned char)s[i + pos]];
      if (!match) break;
      if (pos >= minlen - 1) {
        possible_match = true;
        break;
      }
    }
    bool found = false;
    if (possible_match) {
      found = strtr_slow(arr, result, key, s, i, minlen, maxlen);
    }
    if (!found) {
      result.append(s[i++]);
    }
  }
  return result.detach();
}