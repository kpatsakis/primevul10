pcre_get_compiled_regex_cache(PCRECache::Accessor& accessor,
                              const StringData* regex) {
  PCRECache::TempKeyCache tkc;

  /* Try to lookup the cached regex entry, and if successful, just pass
     back the compiled pattern, otherwise go on and compile it. */
  if (s_pcreCache.find(accessor, regex, tkc)) {
    return true;
  }

  /* Parse through the leading whitespace, and display a warning if we
     get to the end without encountering a delimiter. */
  const char *p = regex->data();
  while (isspace((int)*(unsigned char *)p)) p++;
  if (*p == 0) {
    raise_warning("Empty regular expression");
    return false;
  }

  /* Get the delimiter and display a warning if it is alphanumeric
     or a backslash. */
  char delimiter = *p++;
  if (isalnum((int)*(unsigned char *)&delimiter) || delimiter == '\\') {
    raise_warning("Delimiter must not be alphanumeric or backslash");
    return false;
  }

  char start_delimiter = delimiter;
  const char *pp = strchr("([{< )]}> )]}>", delimiter);
  if (pp) {
    delimiter = pp[5];
  }
  char end_delimiter = delimiter;

  if (start_delimiter == end_delimiter) {
    /* We need to iterate through the pattern, searching for the ending
     * delimiter, but skipping the backslashed delimiters. If the ending
     * delimiter is not found, display a warning. */
    pp = p;
    while (*pp != 0) {
      if (*pp == '\\' && pp[1] != 0) pp++;
      else if (*pp == delimiter)
        break;
      pp++;
    }
    if (*pp == 0) {
      raise_warning("No ending delimiter '%c' found: [%s]", delimiter,
                      regex->data());
      return false;
    }
  } else {
    /* We iterate through the pattern, searching for the matching ending
     * delimiter. For each matching starting delimiter, we increment nesting
     * level, and decrement it for each matching ending delimiter. If we
     * reach the end of the pattern without matching, display a warning.
     */
    int brackets = 1; // brackets nesting level
    pp = p;
    while (*pp != 0) {
      if (*pp == '\\' && pp[1] != 0) pp++;
      else if (*pp == end_delimiter && --brackets <= 0)
        break;
      else if (*pp == start_delimiter)
        brackets++;
      pp++;
    }
    if (*pp == 0) {
      raise_warning("No ending matching delimiter '%c' found: [%s]",
                      end_delimiter, regex->data());
      return false;
    }
  }

  /* Make a copy of the actual pattern. */
  String spattern(p, pp-p, CopyString);
  const char *pattern = spattern.data();

  /* Move on to the options */
  pp++;

  /* Parse through the options, setting appropriate flags.  Display
     a warning if we encounter an unknown modifier. */
  int coptions = 0;
  int poptions = 0;
  bool do_study = false;
  while (*pp != 0) {
    switch (*pp++) {
      /* Perl compatible options */
    case 'i':  coptions |= PCRE_CASELESS;       break;
    case 'm':  coptions |= PCRE_MULTILINE;      break;
    case 's':  coptions |= PCRE_DOTALL;         break;
    case 'x':  coptions |= PCRE_EXTENDED;       break;

      /* PCRE specific options */
    case 'A':  coptions |= PCRE_ANCHORED;       break;
    case 'D':  coptions |= PCRE_DOLLAR_ENDONLY; break;
    case 'S':  do_study = true;                 break;
    case 'U':  coptions |= PCRE_UNGREEDY;       break;
    case 'X':  coptions |= PCRE_EXTRA;          break;
    case 'u':  coptions |= PCRE_UTF8;
  /* In  PCRE,  by  default, \d, \D, \s, \S, \w, and \W recognize only ASCII
       characters, even in UTF-8 mode. However, this can be changed by setting
       the PCRE_UCP option. */
#ifdef PCRE_UCP
            coptions |= PCRE_UCP;
#endif
      break;

      /* Custom preg options */
    case 'e':  poptions |= PREG_REPLACE_EVAL;   break;

    case ' ':
    case '\n':
    case '\r':
      break;

    default:
      raise_warning("Unknown modifier '%c': [%s]", pp[-1], regex->data());
      return false;
    }
  }

  /* We've reached a null byte, now check if we're actually at the end of the
     string.  If not this is a bad expression, and a potential security hole. */
  if (regex->size() != (pp - regex->data())) {
    raise_error("Error: Null byte found in pattern");
  }

  /* Compile pattern and display a warning if compilation failed. */
  const char  *error;
  int erroffset;
  pcre *re = pcre_compile(pattern, coptions, &error, &erroffset, 0);
  if (re == nullptr) {
    raise_warning("Compilation failed: %s at offset %d", error, erroffset);
    return false;
  }

  // Careful: from here 're' needs to be freed if something throws.

  // TODO(t14969501): enable literal_data everywhere and skip the
  // pcre_compile above.
  auto const literal_data = pcre_literal_data(pattern, coptions);

  /* If study option was specified, study the pattern and
     store the result in extra for passing to pcre_exec. */
  pcre_extra *extra = nullptr;
  if (!literal_data.isLiteral()) {
    if (do_study || PCRE_STUDY_JIT_COMPILE) {
      int soptions = PCRE_STUDY_JIT_COMPILE;
      extra = pcre_study(re, soptions, &error);
      if (extra) {
        extra->flags |= PCRE_EXTRA_MATCH_LIMIT |
          PCRE_EXTRA_MATCH_LIMIT_RECURSION;
        pcre_assign_jit_stack(extra, alloc_jit_stack, nullptr);
      }
      if (error != nullptr) {
        try {
          raise_warning("Error while studying pattern");
        } catch (...) {
          pcre_free(re);
          throw;
        }
      }
      if ((!RuntimeOption::EvalJitNoGdb ||
           RuntimeOption::EvalJitUseVtuneAPI ||
           RuntimeOption::EvalPerfPidMap) &&
          extra &&
          extra->executable_jit != nullptr) {
        size_t size;
        pcre_fullinfo(re, extra, PCRE_INFO_JITSIZE, &size);

        TCA start = *(TCA *)(extra->executable_jit);
        TCA end = start + size;
        std::string name = folly::sformat("HHVM::pcre_jit::{}", pattern);

        if (!RuntimeOption::EvalJitNoGdb && jit::mcgen::initialized()) {
          Debug::DebugInfo::Get()->recordStub(Debug::TCRange(start, end, false),
                                              name);
        }
        if (RuntimeOption::EvalJitUseVtuneAPI) {
          HPHP::jit::reportHelperToVtune(name.c_str(), start, end);
        }
        if (RuntimeOption::EvalPerfPidMap && jit::mcgen::initialized()) {
          std::string escaped_name;
          folly::json::escapeString(name, escaped_name,
                                    folly::json::serialization_opts());
          Debug::DebugInfo::Get()->recordPerfMap(
            Debug::TCRange(start, end, false),
            SrcKey{}, escaped_name
          );
        }
      }
    }
  }

  /* Store the compiled pattern and extra info in the cache. */
  pcre_cache_entry* new_entry = new pcre_cache_entry();
  new_entry->re = re;
  new_entry->extra = extra;
  if (literal_data.isLiteral()) {
    new_entry->literal_data =
      std::make_unique<pcre_literal_data>(std::move(literal_data));
  }

  assertx((poptions & ~0x1) == 0);
  new_entry->preg_options = poptions;

  assertx((coptions & 0x80000000) == 0);
  new_entry->compile_options = coptions;

  /* Get pcre full info */
  if (!get_pcre_fullinfo(new_entry)) {
    delete new_entry;
    return false;
  }

  s_pcreCache.insert(accessor, regex, tkc, new_entry);
  return true;
}