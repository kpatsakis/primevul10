Variant strpbrk_impl(const String& haystack, const String& char_list) {
  if (char_list.empty()) {
    raise_invalid_argument_warning("char_list: (empty)");
    return false;
  }
  if (haystack.empty()) {
    return false;
  }

  auto charListData = char_list.c_str();

  // equivalent to rawmemchr(charListData, '\0') ... charListData must be
  // null-terminated
  auto firstNull = charListData;
  while (*firstNull != '\0') { ++firstNull; }

  auto const hasNullByte = (firstNull - charListData) < char_list.size();

  if (UNLIKELY(hasNullByte)) {
    if ((firstNull - charListData) == (char_list.size() - 1)) {
      // the first null is the last character in char_list
    } else if (firstNull == charListData) {
      // the first null is the first character in char_list
      auto secondNull = firstNull + 1;
      while (*secondNull != '\0') { ++secondNull; }

      if ((secondNull - charListData) != char_list.size()) {
        return
          strpbrk_char_list_has_nulls_slow<existence_only>(haystack, char_list);
      }
      ++charListData; // we can remember the null byte
    } else {
      return
        strpbrk_char_list_has_nulls_slow<existence_only>(haystack, char_list);
    }
  }

  // Use strcspn instead of strpbrk because the latter doesn't report when
  // it's terminated due to a null byte in haystack in any manageable way.
  auto haySize = haystack.size();
  auto hayData = haystack.c_str();
retry:
  size_t idx = strcspn(hayData, charListData);
  if (idx < haySize) {
    if (UNLIKELY(hayData[idx] == '\0' && !hasNullByte)) {
      hayData += idx + 1;
      haySize -= idx + 1;
      goto retry;
    }
    if (existence_only) { return true; }

    return String(hayData + idx, haySize - idx, CopyString);
  }
  return false;
}