Variant strpbrk_char_list_has_nulls_slow(const String& haystack,
                                         const String& char_list) {

  auto const charListSz = char_list.size();
  auto const charListData = char_list.c_str();
  assertx(memchr(charListData, '\0', charListSz) != nullptr);

  // in order to use strcspn, remove all null byte(s) from char_list
  auto charListWithoutNull = (char*) req::malloc_noptrs(charListSz);
  SCOPE_EXIT { req::free(charListWithoutNull); };

  auto copy_ptr = charListWithoutNull;
  auto const charListStop = charListData + char_list.size();
  for (auto ptr = charListData; ptr != charListStop; ++ptr) {
    if (*ptr != '\0') { *copy_ptr++ = *ptr; }
  }
  assertx((copy_ptr - charListWithoutNull) < charListSz);
  // at least one of charListData chars was null, so there must be room:
  *copy_ptr = '\0';

  // Use strcspn instead of strpbrk because the latter doesn't report when
  // its terminated due to a null byte in haystack in any manageable way.
  auto haySize = haystack.size();
  auto hayData = haystack.c_str();

  size_t idx = strcspn(hayData, charListWithoutNull);
  if (idx < haySize) {
    // we know that char_list contains null bytes, being terminated because
    // haystack has null bytes is just dandy
    if (existence_only) { return true; }
    return String(hayData + idx, haySize - idx, CopyString);
  }
  return false;
}