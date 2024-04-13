Variant preg_split(const String& pattern, const String& subject,
                   int limit /* = -1 */, int flags /* = 0 */) {
  PCRECache::Accessor accessor;
  if (!pcre_get_compiled_regex_cache(accessor, pattern.get())) {
    return preg_return_bad_regex_error(false);
  }
  const pcre_cache_entry* pce = accessor.get();

  int no_empty = flags & PREG_SPLIT_NO_EMPTY;
  bool delim_capture = flags & PREG_SPLIT_DELIM_CAPTURE;
  bool offset_capture = flags & PREG_SPLIT_OFFSET_CAPTURE;

  if (limit == 0) {
    limit = -1;
  }

  int size_offsets = 0;
  int* offsets = create_offset_array(pce, size_offsets);
  SmartFreeHelper offsetsFreer(offsets);
  if (offsets == nullptr) {
    return preg_return_internal_error(false);
  }

  /* Start at the beginning of the string */
  int start_offset = 0;
  int next_offset = 0;
  const char* last_match = subject.data();
  pcre_extra extra;
  init_local_extra(&extra, pce->extra);

  const bool hackArrOutput = flags & PREG_FB_HACK_ARRAYS;

  // Get next piece if no limit or limit not yet reached and something matched
  Array result = hackArrOutput ? Array::CreateDict() : Array::CreateDArray();
  int g_notempty = 0;   /* If the match should not be empty */
  int utf8_check = 0;
  PCRECache::Accessor bump_accessor;
  const pcre_cache_entry* bump_pce = nullptr; /* instance for empty matches */
  while ((limit == -1 || limit > 1)) {
    int count = pcre_exec(pce->re, &extra, subject.data(), subject.size(),
                          start_offset, g_notempty | utf8_check,
                          offsets, size_offsets);

    /* Subsequent calls to pcre_exec don't need to bother with the
     * utf8 validity check: if the subject isn't valid, the first
     * call to pcre_exec will have failed, and as long as we only
     * set start_offset to known character boundaries we won't
     * supply an invalid offset. */
    utf8_check = PCRE_NO_UTF8_CHECK;

    /* Check for too many substrings condition. */
    if (count == 0) {
      raise_warning("Matched, but too many substrings");
      count = size_offsets / 3;
    }

    /* If something matched */
    if (count > 0 && offsets[1] >= offsets[0]) {
      if (!no_empty || subject.data() + offsets[0] != last_match) {
        auto const length = subject.data() + offsets[0] - last_match;
        auto const match = String(last_match, length, CopyString);
        auto const value = offset_capture
          ? Variant(str_offset_pair(match, next_offset, hackArrOutput))
          : Variant(match);
        assertx(result->isVectorData());
        result.set(safe_cast<int64_t>(result.size()), value);

        /* One less left to do */
        if (limit != -1) limit--;
      }

      last_match = subject.data() + offsets[1];
      next_offset = offsets[1];

      if (delim_capture) {
        int i, match_len;
        for (i = 1; i < count; i++) {
          match_len = offsets[(i<<1)+1] - offsets[i<<1];
          /* If we have matched a delimiter */
          if (!no_empty || match_len > 0) {
            auto const match = subject.substr(offsets[i<<1], match_len);
            auto const value = offset_capture
              ? Variant(str_offset_pair(match, offsets[i<<1], hackArrOutput))
              : Variant(match);
            assertx(result->isVectorData());
            result.set(safe_cast<int64_t>(result.size()), value);
          }
        }
      }
    } else if (count == PCRE_ERROR_NOMATCH) {
      /* If we previously set PCRE_NOTEMPTY after a null match,
         this is not necessarily the end. We need to advance
         the start offset, and continue. Fudge the offset values
         to achieve this, unless we're already at the end of the string. */
      if (g_notempty != 0 && start_offset < subject.size()) {
        if (pce->compile_options & PCRE_UTF8) {
          if (bump_pce == nullptr) {
            auto const DEBUG_ONLY ok = pcre_get_compiled_regex_cache(
              bump_accessor, s_OneUnicodeCharPattern.get());
            assertx(ok);
            bump_pce = bump_accessor.get();
          }
          pcre_extra bump_extra;
          init_local_extra(&bump_extra, bump_pce->extra);
          count = pcre_exec(bump_pce->re, &bump_extra, subject.data(),
                            subject.size(), start_offset,
                            utf8_check, offsets, size_offsets);
          if (count < 1) {
            raise_warning("Unknown error");
            offsets[0] = start_offset;
            offsets[1] = start_offset + 1;
            if (pcre_need_log_error(count)) {
              pcre_log_error(__FUNCTION__, __LINE__, count,
                             pattern.data(), pattern.size(),
                             subject.data(), subject.size(),
                             "", 0,
                             limit, flags, start_offset);
            }
          }
        } else {
          offsets[0] = start_offset;
          offsets[1] = start_offset + 1;
        }
      } else
        break;
    } else {
      if (pcre_need_log_error(count)) {
        pcre_log_error(__FUNCTION__, __LINE__, count,
                       pattern.data(), pattern.size(),
                       subject.data(), subject.size(),
                       "", 0,
                       limit, flags, start_offset, g_notempty);
      }
      // NOTE: this returns an error together with a partial result :-(
      start_offset = last_match - subject.data(); /* offset might have
                                                   * been incremented,
                                                   * but without further
                                                   * successful matches */
      if (!no_empty || start_offset < subject.size()) {
        auto const match = subject.substr(start_offset);
        auto const value = offset_capture
          ? Variant(str_offset_pair(match, start_offset, hackArrOutput))
          : Variant(match);
        assertx(result->isVectorData());
        result.set(safe_cast<int64_t>(result.size()), value);
      }
      return preg_return_pcre_error(count, std::move(result));
    }

    /* If we have matched an empty string, mimic what Perl's /g options does.
       This turns out to be rather cunning. First we set PCRE_NOTEMPTY and try
       the match again at the same point. If this fails (picked up above) we
       advance to the next character. */
    g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY | PCRE_ANCHORED : 0;

    /* Advance to the position right after the last full match */
    start_offset = offsets[1];
  }

  start_offset = last_match - subject.data(); /* offset might have
                                                * been incremented,
                                                * but without further
                                                * successful matches */
  if (!no_empty || start_offset < subject.size()) {
    auto const match = subject.substr(start_offset);
    auto const value = offset_capture
      ? Variant(str_offset_pair(match, start_offset, hackArrOutput))
      : Variant(match);
    assertx(result->isVectorData());
    result.set(safe_cast<int64_t>(result.size()), value);
  }

  return preg_return_no_error(std::move(result));
}