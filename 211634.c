Variant preg_grep(const String& pattern, const Array& input, int flags /* = 0 */) {
  PCRECache::Accessor accessor;
  if (!pcre_get_compiled_regex_cache(accessor, pattern.get())) {
    return preg_return_bad_regex_error(false);
  }
  const pcre_cache_entry* pce = accessor.get();

  int size_offsets = 0;
  int* offsets = create_offset_array(pce, size_offsets);
  if (offsets == nullptr) {
    return preg_return_internal_error(false);
  }
  SmartFreeHelper freer(offsets);

  const bool hackArrOutput = flags & PREG_FB_HACK_ARRAYS;

  /* Initialize return array */
  auto ret = hackArrOutput ? Array::CreateDict() : Array::CreateDArray();

  /* Go through the input array */
  bool invert = (flags & PREG_GREP_INVERT);
  pcre_extra extra;
  init_local_extra(&extra, pce->extra);

  for (ArrayIter iter(input); iter; ++iter) {
    String entry = iter.second().toString();

    /* Perform the match */
    int count = pcre_exec(pce->re, &extra, entry.data(), entry.size(),
                          0, 0, offsets, size_offsets);

    /* Check for too many substrings condition. */
    if (count == 0) {
      raise_warning("Matched, but too many substrings");
      count = size_offsets / 3;
    } else if (count < 0 && count != PCRE_ERROR_NOMATCH) {
      if (pcre_need_log_error(count)) {
        pcre_log_error(__FUNCTION__, __LINE__, count,
                       pattern.data(), pattern.size(),
                       entry.data(), entry.size(),
                       "", 0,
                       flags);
      }
      // NOTE: this returns an error together with a partial result :-(
      return preg_return_pcre_error(count, std::move(ret));
    }

    /* If the entry fits our requirements */
    if ((count > 0 && !invert) ||
        (count == PCRE_ERROR_NOMATCH && invert)) {

      /* Add to return array */
      ret.set(iter.first(), entry);
    }
  }

  return preg_return_no_error(std::move(ret));
}