static Variant php_pcre_replace(const String& pattern, const String& subject,
                                const Variant& replace_var, bool callable,
                                int limit, int* replace_count) {
  PCRECache::Accessor accessor;
  if (!pcre_get_compiled_regex_cache(accessor, pattern.get())) {
    return preg_return_bad_regex_error(init_null());
  }
  const pcre_cache_entry* pce = accessor.get();
  if (pce->preg_options & PREG_REPLACE_EVAL) {
    raise_error("preg_replace(): Support for the /e modifier has been removed, use "
                "preg_replace_callback instead");
  }

  int size_offsets;
  int* offsets = create_offset_array(pce, size_offsets);
  SmartFreeHelper offsetsFreer(offsets);
  if (offsets == nullptr) {
    return preg_return_internal_error(init_null());
  }

  const char* const* subpat_names = get_subpat_names(pce);
  if (subpat_names == nullptr) {
    return preg_return_internal_error(init_null());
  }

  const char* replace = nullptr;
  const char* replace_end = nullptr;
  int replace_len = 0;
  String replace_val;

  if (!callable) {
    replace_val = replace_var.toString();
    replace = replace_val.data();
    replace_len = replace_val.size();
    replace_end = replace + replace_len;
  }

  StringBuffer result(2 * subject.size());

  try {

    /* Initialize */
    const char* match = nullptr;
    int start_offset = 0;
    pcre_extra extra;
    init_local_extra(&extra, pce->extra);

    const char* walk;     // Used to walk the replacement string
    char walk_last;       // Last walked character
    int match_len;        // Length of the current match
    int backref;          // Backreference number
    int g_notempty = 0;   // If the match should not be empty
    int exec_options = 0; // Options passed to pcre_exec
    while (1) {
      /* Execute the regular expression. */
      int count = pcre_exec(pce->re, &extra, subject.data(), subject.size(),
                            start_offset,
                            exec_options | g_notempty,
                            offsets, size_offsets);

      /* The string was already proved to be valid UTF-8 */
      exec_options |= PCRE_NO_UTF8_CHECK;

      /* Check for too many substrings condition. */
      if (count == 0) {
        raise_warning("Matched, but too many substrings");
        count = size_offsets / 3;
      }

      const char* piece = subject.data() + start_offset;
      if (count > 0 && offsets[1] >= offsets[0] &&
          (limit == -1 || limit > 0)) {
        if (replace_count) {
          ++*replace_count;
        }
        /* Set the match location in subject */
        match = subject.data() + offsets[0];

        String callable_result;
        if (callable) {
          /* Use custom function to get replacement string and its length. */
          callable_result = preg_do_repl_func(replace_var, subject, offsets,
                                              subpat_names, count);
        } else { /* do regular substitution */
          walk = replace;
          walk_last = 0;
          while (walk < replace_end) {
            if ('\\' == *walk || '$' == *walk) {
              if (walk_last == '\\') {
                walk++;
                walk_last = 0;
                continue;
              }
              if (preg_get_backref(&walk, &backref)) {
                if (backref < count) {
                  match_len = offsets[(backref<<1)+1] - offsets[backref<<1];
                }
                continue;
              }
            }
            walk++;
            walk_last = walk[-1];
          }
        }

        /* copy the part of the string before the match */
        result.append(piece, match-piece);

        /* copy replacement and backrefs */
        int result_len = result.size();

        if (callable) {
          /* Copy result from custom function to buffer and clean up. */
          result.append(callable_result.data(), callable_result.size());
          result_len += callable_result.size();
        } else { /* do regular backreference copying */
          walk = replace;
          walk_last = 0;
          Array params;
          while (walk < replace_end) {
            if ('\\' == *walk || '$' == *walk) {
              if (walk_last == '\\') {
                result.set(result.size() - 1, *walk++);
                walk_last = 0;
                continue;
              }
              if (preg_get_backref(&walk, &backref)) {
                if (backref < count) {
                  match_len = offsets[(backref<<1)+1] - offsets[backref<<1];
                  result.append(
                    subject.data() + offsets[backref<<1],
                    match_len
                  );
                }
                continue;
              }
            }
            result.append(*walk++);
            walk_last = walk[-1];
          }
        }

        if (limit != -1) {
          limit--;
        }

      } else if (count == PCRE_ERROR_NOMATCH || limit == 0) {
        /* If we previously set PCRE_NOTEMPTY after a null match,
           this is not necessarily the end. We need to advance
           the start offset, and continue. Fudge the offset values
           to achieve this, unless we're already at the end of the string. */
        if (g_notempty != 0 && start_offset < subject.size()) {
          offsets[0] = start_offset;
          offsets[1] = start_offset + 1;
          result.append(piece, 1);
        } else {
          /* stick that last bit of string on our output */
          result.append(piece, subject.size() - start_offset);
          break;
        }
      } else {
        if (pcre_need_log_error(count)) {
          const char* s;
          int size;
          String stemp;
          if (callable) {
            if (replace_var.isObject()) {
              stemp = replace_var.asCObjRef()->getClassName().asString()
                    + "::__invoke";
            } else {
              stemp = replace_var.toString();
            }
            s = stemp.data();
            size = stemp.size();
          } else {
            s = replace_val.data();
            size = replace_val.size();
          }
          pcre_log_error(__FUNCTION__, __LINE__, count,
                         pattern.data(), pattern.size(),
                         subject.data(), subject.size(),
                         s, size,
                         callable, limit, start_offset, g_notempty);
        }
        return preg_return_pcre_error(count, init_null());
      }

      /* If we have matched an empty string, mimic what Perl's /g options does.
         This turns out to be rather cunning. First we set PCRE_NOTEMPTY and try
         the match again at the same point. If this fails (picked up above) we
         advance to the next character. */
      g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY | PCRE_ANCHORED : 0;

      /* Advance to the next piece. */
      start_offset = offsets[1];
    }

    return preg_return_no_error(result.detach());
  } catch (...) {
    throw;
  }
}